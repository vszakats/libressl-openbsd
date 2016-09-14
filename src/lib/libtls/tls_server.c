/* $OpenBSD: tls_server.c,v 1.28 2016/09/14 11:34:37 bcook Exp $ */
/*
 * Copyright (c) 2014 Joel Sing <jsing@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/socket.h>

#include <arpa/inet.h>

#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <tls.h>
#include "tls_internal.h"

struct tls *
tls_server(void)
{
	struct tls *ctx;

	if ((ctx = tls_new()) == NULL)
		return (NULL);

	ctx->flags |= TLS_SERVER;

	return (ctx);
}

struct tls *
tls_server_conn(struct tls *ctx)
{
	struct tls *conn_ctx;

	if ((conn_ctx = tls_new()) == NULL)
		return (NULL);

	conn_ctx->flags |= TLS_SERVER_CONN;

	return (conn_ctx);
}

static int
tls_server_alpn_cb(SSL *ssl, const unsigned char **out, unsigned char *outlen,
    const unsigned char *in, unsigned int inlen, void *arg)
{
	struct tls *ctx = arg;

	if (SSL_select_next_proto((unsigned char**)out, outlen,
	    ctx->config->alpn, ctx->config->alpn_len, in, inlen) ==
	    OPENSSL_NPN_NEGOTIATED)
		return (SSL_TLSEXT_ERR_OK);

	return (SSL_TLSEXT_ERR_NOACK);
}

static int
tls_servername_cb(SSL *ssl, int *al, void *arg)
{
	struct tls *ctx = (struct tls *)arg;
	struct tls_sni_ctx *sni_ctx;
	union tls_addr addrbuf;
	struct tls *conn_ctx;
	const char *name;

	if ((conn_ctx = SSL_get_app_data(ssl)) == NULL)
		goto err;

	if ((name = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name)) == NULL) {
		/*
		 * The servername callback gets called even when there is no
		 * TLS servername extension provided by the client. Sigh!
		 */
		return (SSL_TLSEXT_ERR_NOACK);
	}

	/* Per RFC 6066 section 3: ensure that name is not an IP literal. */
	if (inet_pton(AF_INET, name, &addrbuf) == 1 ||
            inet_pton(AF_INET6, name, &addrbuf) == 1)
		goto err;

	free((char *)conn_ctx->servername);
	if ((conn_ctx->servername = strdup(name)) == NULL)
		goto err;

	/* Find appropriate SSL context for requested servername. */
	for (sni_ctx = ctx->sni_ctx; sni_ctx != NULL; sni_ctx = sni_ctx->next) {
		if (tls_check_name(ctx, sni_ctx->ssl_cert, name) == 0) {
			SSL_set_SSL_CTX(conn_ctx->ssl_conn, sni_ctx->ssl_ctx);
			return (SSL_TLSEXT_ERR_OK);
		}
	}

	/* No match, use the existing context/certificate. */
	return (SSL_TLSEXT_ERR_OK);

 err:
	/*
	 * There is no way to tell libssl that an internal failure occurred.
	 * The only option we have is to return a fatal alert.
	 */
	*al = TLS1_AD_INTERNAL_ERROR;
	return (SSL_TLSEXT_ERR_ALERT_FATAL);
}

static int
tls_keypair_load_cert(struct tls_keypair *keypair, struct tls_error *error,
    X509 **cert)
{
	char *errstr = "unknown";
	BIO *cert_bio = NULL;
	int ssl_err;

	X509_free(*cert);
	*cert = NULL;

	if (keypair->cert_mem == NULL) {
		tls_error_set(error, "keypair has no certificate");
		goto err;
	}
	if ((cert_bio = BIO_new_mem_buf(keypair->cert_mem,
	    keypair->cert_len)) == NULL) {
		tls_error_set(error, "failed to create certificate bio");
		goto err;
	}
	if ((*cert = PEM_read_bio_X509(cert_bio, NULL, NULL, NULL)) == NULL) {
		if ((ssl_err = ERR_peek_error()) != 0)
		    errstr = ERR_error_string(ssl_err, NULL);
		tls_error_set(error, "failed to load certificate: %s", errstr);
		goto err;
	}

	BIO_free(cert_bio);

	return (0);

 err:
	BIO_free(cert_bio);

	return (-1);
}

static int
tls_configure_server_ssl(struct tls *ctx, SSL_CTX **ssl_ctx,
    struct tls_keypair *keypair)
{
	unsigned char sid[SSL_MAX_SSL_SESSION_ID_LENGTH];
	EC_KEY *ecdh_key;

	SSL_CTX_free(*ssl_ctx);

	if ((*ssl_ctx = SSL_CTX_new(SSLv23_server_method())) == NULL) {
		tls_set_errorx(ctx, "ssl context failure");
		goto err;
	}

	if (SSL_CTX_set_tlsext_servername_callback(*ssl_ctx,
	    tls_servername_cb) != 1) {
		tls_set_error(ctx, "failed to set servername callback");
		goto err;
	}
	if (SSL_CTX_set_tlsext_servername_arg(*ssl_ctx, ctx) != 1) {
		tls_set_error(ctx, "failed to set servername callback arg");
		goto err;
	}

	if (tls_configure_ssl(ctx, *ssl_ctx) != 0)
		goto err;
	if (tls_configure_ssl_keypair(ctx, *ssl_ctx, keypair, 1) != 0)
		goto err;
	if (ctx->config->verify_client != 0) {
		int verify = SSL_VERIFY_PEER;
		if (ctx->config->verify_client == 1)
			verify |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
		if (tls_configure_ssl_verify(ctx, *ssl_ctx, verify) == -1)
			goto err;
	}

	if (ctx->config->alpn != NULL)
		SSL_CTX_set_alpn_select_cb(*ssl_ctx, tls_server_alpn_cb,
		    ctx);

	if (ctx->config->dheparams == -1)
		SSL_CTX_set_dh_auto(*ssl_ctx, 1);
	else if (ctx->config->dheparams == 1024)
		SSL_CTX_set_dh_auto(*ssl_ctx, 2);

	if (ctx->config->ecdhecurve == -1) {
		SSL_CTX_set_ecdh_auto(*ssl_ctx, 1);
	} else if (ctx->config->ecdhecurve != NID_undef) {
		if ((ecdh_key = EC_KEY_new_by_curve_name(
		    ctx->config->ecdhecurve)) == NULL) {
			tls_set_errorx(ctx, "failed to set ECDHE curve");
			goto err;
		}
		SSL_CTX_set_options(*ssl_ctx, SSL_OP_SINGLE_ECDH_USE);
		SSL_CTX_set_tmp_ecdh(*ssl_ctx, ecdh_key);
		EC_KEY_free(ecdh_key);
	}

	if (ctx->config->ciphers_server == 1)
		SSL_CTX_set_options(*ssl_ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);

	/*
	 * Set session ID context to a random value.  We don't support
	 * persistent caching of sessions so it is OK to set a temporary
	 * session ID context that is valid during run time.
	 */
	arc4random_buf(sid, sizeof(sid));
	if (SSL_CTX_set_session_id_context(*ssl_ctx, sid,
	    sizeof(sid)) != 1) {
		tls_set_error(ctx, "failed to set session id context");
		goto err;
	}

	return (0);

  err:
	SSL_CTX_free(*ssl_ctx);
	*ssl_ctx = NULL;

	return (-1);
}

static int
tls_configure_server_sni(struct tls *ctx)
{
	struct tls_sni_ctx **sni_ctx;
	struct tls_keypair *kp;

	if (ctx->config->keypair->next == NULL)
		return (0);

	/* Set up additional SSL contexts for SNI. */
	sni_ctx = &ctx->sni_ctx;
	for (kp = ctx->config->keypair->next; kp != NULL; kp = kp->next) {
		if ((*sni_ctx = tls_sni_ctx_new()) == NULL) {
			tls_set_errorx(ctx, "out of memory");
			goto err;
		}
		if (tls_configure_server_ssl(ctx, &(*sni_ctx)->ssl_ctx, kp) == -1)
			goto err;
		if (tls_keypair_load_cert(kp, &ctx->error,
		    &(*sni_ctx)->ssl_cert) == -1)
			goto err;
		sni_ctx = &(*sni_ctx)->next;
	}

	return (0);

 err:
	return (-1);
}

int
tls_configure_server(struct tls *ctx)
{
	if (tls_configure_server_ssl(ctx, &ctx->ssl_ctx,
	    ctx->config->keypair) == -1)
		goto err;
	if (tls_configure_server_sni(ctx) == -1)
		goto err;

	return (0);

 err:
	return (-1);
}

static struct tls *
tls_accept_common(struct tls *ctx)
{
	struct tls *conn_ctx = NULL;

	if ((ctx->flags & TLS_SERVER) == 0) {
		tls_set_errorx(ctx, "not a server context");
		goto err;
	}

	if ((conn_ctx = tls_server_conn(ctx)) == NULL) {
		tls_set_errorx(ctx, "connection context failure");
		goto err;
	}

	if ((conn_ctx->ssl_conn = SSL_new(ctx->ssl_ctx)) == NULL) {
		tls_set_errorx(ctx, "ssl failure");
		goto err;
	}

	if (SSL_set_app_data(conn_ctx->ssl_conn, conn_ctx) != 1) {
		tls_set_errorx(ctx, "ssl application data failure");
		goto err;
	}

	return conn_ctx;

 err:
	tls_free(conn_ctx);

	return (NULL);
}

int
tls_accept_socket(struct tls *ctx, struct tls **cctx, int socket)
{
	return (tls_accept_fds(ctx, cctx, socket, socket));
}

int
tls_accept_fds(struct tls *ctx, struct tls **cctx, int fd_read, int fd_write)
{
	struct tls *conn_ctx;

	if ((conn_ctx = tls_accept_common(ctx)) == NULL)
		goto err;

	if (SSL_set_rfd(conn_ctx->ssl_conn, fd_read) != 1 ||
	    SSL_set_wfd(conn_ctx->ssl_conn, fd_write) != 1) {
		tls_set_errorx(ctx, "ssl file descriptor failure");
		goto err;
	}

	*cctx = conn_ctx;

	return (0);
 err:
	tls_free(conn_ctx);
	*cctx = NULL;

	return (-1);
}

int
tls_accept_cbs(struct tls *ctx, struct tls **cctx,
    tls_read_cb read_cb, tls_write_cb write_cb, void *cb_arg)
{
	struct tls *conn_ctx;

	if ((conn_ctx = tls_accept_common(ctx)) == NULL)
		goto err;

	if (tls_set_cbs(conn_ctx, read_cb, write_cb, cb_arg) != 0) {
		tls_set_errorx(ctx, "callback registration failure");
		goto err;
	}

	*cctx = conn_ctx;

	return (0);
 err:
	tls_free(conn_ctx);
	*cctx = NULL;

	return (-1);
}

int
tls_handshake_server(struct tls *ctx)
{
	int ssl_ret;
	int rv = -1;

	if ((ctx->flags & TLS_SERVER_CONN) == 0) {
		tls_set_errorx(ctx, "not a server connection context");
		goto err;
	}

	ERR_clear_error();
	if ((ssl_ret = SSL_accept(ctx->ssl_conn)) != 1) {
		rv = tls_ssl_error(ctx, ctx->ssl_conn, ssl_ret, "handshake");
		goto err;
	}

	ctx->state |= TLS_HANDSHAKE_COMPLETE;
	rv = 0;

 err:
	return (rv);
}
