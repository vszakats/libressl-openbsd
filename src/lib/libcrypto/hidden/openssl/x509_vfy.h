/* $OpenBSD: x509_vfy.h,v 1.5 2023/07/03 11:43:15 tobhe Exp $ */
/*
 * Copyright (c) 2022 Bob Beck <beck@openbsd.org>
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

#ifndef _LIBCRYPTO_X509_VFY_H
#define _LIBCRYPTO_X509_VFY_H

#include_next <openssl/x509_vfy.h>
#include "crypto_namespace.h"

LCRYPTO_USED(X509_STORE_set_depth);
LCRYPTO_USED(X509_STORE_CTX_set_depth);
LCRYPTO_USED(X509_OBJECT_new);
LCRYPTO_USED(X509_OBJECT_free);
LCRYPTO_USED(X509_OBJECT_idx_by_subject);
LCRYPTO_USED(X509_OBJECT_retrieve_by_subject);
LCRYPTO_USED(X509_OBJECT_retrieve_match);
LCRYPTO_USED(X509_OBJECT_up_ref_count);
LCRYPTO_USED(X509_OBJECT_get_type);
LCRYPTO_USED(X509_OBJECT_get0_X509);
LCRYPTO_USED(X509_OBJECT_get0_X509_CRL);
LCRYPTO_USED(X509_STORE_new);
LCRYPTO_USED(X509_STORE_free);
LCRYPTO_USED(X509_STORE_up_ref);
LCRYPTO_USED(X509_STORE_get0_objects);
LCRYPTO_USED(X509_STORE_get_ex_data);
LCRYPTO_USED(X509_STORE_set_ex_data);
LCRYPTO_USED(X509_STORE_set_flags);
LCRYPTO_USED(X509_STORE_set_purpose);
LCRYPTO_USED(X509_STORE_set_trust);
LCRYPTO_USED(X509_STORE_set1_param);
LCRYPTO_USED(X509_STORE_get0_param);
LCRYPTO_USED(X509_STORE_get_verify_cb);
LCRYPTO_USED(X509_STORE_set_verify_cb);
LCRYPTO_USED(X509_STORE_get_check_issued);
LCRYPTO_USED(X509_STORE_set_check_issued);
LCRYPTO_USED(X509_STORE_CTX_get_check_issued);
LCRYPTO_USED(X509_STORE_CTX_new);
LCRYPTO_USED(X509_STORE_CTX_get1_issuer);
LCRYPTO_USED(X509_STORE_CTX_free);
LCRYPTO_USED(X509_STORE_CTX_init);
LCRYPTO_USED(X509_STORE_CTX_get0_cert);
LCRYPTO_USED(X509_STORE_CTX_get0_chain);
LCRYPTO_USED(X509_STORE_CTX_get0_store);
LCRYPTO_USED(X509_STORE_CTX_get0_untrusted);
LCRYPTO_USED(X509_STORE_CTX_set0_untrusted);
LCRYPTO_USED(X509_STORE_CTX_get1_certs);
LCRYPTO_USED(X509_STORE_CTX_get1_crls);
LCRYPTO_USED(X509_STORE_CTX_trusted_stack);
LCRYPTO_USED(X509_STORE_CTX_set0_trusted_stack);
LCRYPTO_USED(X509_STORE_CTX_cleanup);
LCRYPTO_USED(X509_STORE_add_lookup);
LCRYPTO_USED(X509_LOOKUP_hash_dir);
LCRYPTO_USED(X509_LOOKUP_file);
LCRYPTO_USED(X509_LOOKUP_mem);
LCRYPTO_USED(X509_STORE_add_cert);
LCRYPTO_USED(X509_STORE_add_crl);
LCRYPTO_USED(X509_STORE_CTX_get_by_subject);
LCRYPTO_USED(X509_STORE_CTX_get_obj_by_subject);
LCRYPTO_USED(X509_LOOKUP_ctrl);
LCRYPTO_USED(X509_load_cert_file);
LCRYPTO_USED(X509_load_crl_file);
LCRYPTO_USED(X509_load_cert_crl_file);
LCRYPTO_USED(X509_LOOKUP_new);
LCRYPTO_USED(X509_LOOKUP_free);
LCRYPTO_USED(X509_LOOKUP_init);
LCRYPTO_USED(X509_LOOKUP_by_subject);
LCRYPTO_USED(X509_LOOKUP_by_issuer_serial);
LCRYPTO_USED(X509_LOOKUP_by_fingerprint);
LCRYPTO_USED(X509_LOOKUP_by_alias);
LCRYPTO_USED(X509_LOOKUP_shutdown);
LCRYPTO_USED(X509_STORE_load_locations);
LCRYPTO_USED(X509_STORE_load_mem);
LCRYPTO_USED(X509_STORE_set_default_paths);
LCRYPTO_USED(X509_STORE_CTX_get_ex_new_index);
LCRYPTO_USED(X509_STORE_CTX_set_ex_data);
LCRYPTO_USED(X509_STORE_CTX_get_ex_data);
LCRYPTO_USED(X509_STORE_CTX_get_error);
LCRYPTO_USED(X509_STORE_CTX_set_error);
LCRYPTO_USED(X509_STORE_CTX_get_error_depth);
LCRYPTO_USED(X509_STORE_CTX_set_error_depth);
LCRYPTO_USED(X509_STORE_CTX_get_current_cert);
LCRYPTO_USED(X509_STORE_CTX_set_current_cert);
LCRYPTO_USED(X509_STORE_CTX_get0_current_issuer);
LCRYPTO_USED(X509_STORE_CTX_get0_current_crl);
LCRYPTO_USED(X509_STORE_CTX_get0_parent_ctx);
LCRYPTO_USED(X509_STORE_CTX_get_chain);
LCRYPTO_USED(X509_STORE_CTX_get1_chain);
LCRYPTO_USED(X509_STORE_CTX_set_cert);
LCRYPTO_USED(X509_STORE_CTX_set_chain);
LCRYPTO_USED(X509_STORE_CTX_set0_crls);
LCRYPTO_USED(X509_STORE_CTX_set_purpose);
LCRYPTO_USED(X509_STORE_CTX_set_trust);
LCRYPTO_USED(X509_STORE_CTX_purpose_inherit);
LCRYPTO_USED(X509_STORE_CTX_set_flags);
LCRYPTO_USED(X509_STORE_CTX_set_time);
LCRYPTO_USED(X509_STORE_CTX_set0_verified_chain);
LCRYPTO_USED(X509_STORE_CTX_get_verify);
LCRYPTO_USED(X509_STORE_CTX_set_verify);
LCRYPTO_USED(X509_STORE_CTX_get_verify_cb);
LCRYPTO_USED(X509_STORE_CTX_set_verify_cb);
LCRYPTO_USED(X509_STORE_set_verify);
LCRYPTO_USED(X509_STORE_get_verify);
LCRYPTO_USED(X509_STORE_CTX_get_num_untrusted);
LCRYPTO_USED(X509_STORE_CTX_get0_param);
LCRYPTO_USED(X509_STORE_CTX_set0_param);
LCRYPTO_USED(X509_STORE_CTX_set_default);
LCRYPTO_USED(X509_VERIFY_PARAM_new);
LCRYPTO_USED(X509_VERIFY_PARAM_free);
LCRYPTO_USED(X509_VERIFY_PARAM_inherit);
LCRYPTO_USED(X509_VERIFY_PARAM_set1);
LCRYPTO_USED(X509_VERIFY_PARAM_set1_name);
LCRYPTO_USED(X509_VERIFY_PARAM_set_flags);
LCRYPTO_USED(X509_VERIFY_PARAM_clear_flags);
LCRYPTO_USED(X509_VERIFY_PARAM_get_flags);
LCRYPTO_USED(X509_VERIFY_PARAM_set_purpose);
LCRYPTO_USED(X509_VERIFY_PARAM_set_trust);
LCRYPTO_USED(X509_VERIFY_PARAM_set_depth);
LCRYPTO_USED(X509_VERIFY_PARAM_set_auth_level);
LCRYPTO_USED(X509_VERIFY_PARAM_get_time);
LCRYPTO_USED(X509_VERIFY_PARAM_set_time);
LCRYPTO_USED(X509_VERIFY_PARAM_add0_policy);
LCRYPTO_USED(X509_VERIFY_PARAM_set1_policies);
LCRYPTO_USED(X509_VERIFY_PARAM_get_depth);
LCRYPTO_USED(X509_VERIFY_PARAM_set1_host);
LCRYPTO_USED(X509_VERIFY_PARAM_add1_host);
LCRYPTO_USED(X509_VERIFY_PARAM_set_hostflags);
LCRYPTO_USED(X509_VERIFY_PARAM_get0_peername);
LCRYPTO_USED(X509_VERIFY_PARAM_set1_email);
LCRYPTO_USED(X509_VERIFY_PARAM_set1_ip);
LCRYPTO_USED(X509_VERIFY_PARAM_set1_ip_asc);
LCRYPTO_USED(X509_VERIFY_PARAM_get0_name);
LCRYPTO_USED(X509_VERIFY_PARAM_get0);
LCRYPTO_USED(X509_VERIFY_PARAM_get_count);
LCRYPTO_USED(X509_VERIFY_PARAM_add0_table);
LCRYPTO_USED(X509_VERIFY_PARAM_lookup);
LCRYPTO_USED(X509_VERIFY_PARAM_table_cleanup);

#endif /* _LIBCRYPTO_X509_VFY_H */
