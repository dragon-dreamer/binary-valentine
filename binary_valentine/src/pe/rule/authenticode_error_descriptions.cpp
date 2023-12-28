#include "binary_valentine/pe/rule/authenticode_error_descriptions.h"

#include "pe_bliss2/security/authenticode_certificate_store.h"
#include "pe_bliss2/security/authenticode_check_status.h"
#include "pe_bliss2/security/authenticode_format_validator_errc.h"
#include "pe_bliss2/security/authenticode_timestamp_signature_format_validator.h"
#include "pe_bliss2/security/crypto_algorithms.h"
#include "pe_bliss2/security/pkcs7/pkcs7_format_validator.h"
#include "pe_bliss2/security/pkcs7/signer_info.h"
#include "pe_bliss2/security/signature_verifier.h"

namespace bv::pe
{

namespace
{

static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
{
	using namespace pe_bliss::security;
	using namespace pe_bliss::security::pkcs7;

	return {
		{ crypto_algorithm_errc::signature_hash_and_digest_algorithm_mismatch,
			"pe_authenticode_signature_hash_and_digest_algorithm_mismatch" },
		{ crypto_algorithm_errc::unsupported_digest_algorithm,
			"pe_authenticode_unsupported_digest_algorithm" },
		{ crypto_algorithm_errc::unsupported_digest_encryption_algorithm,
			"pe_authenticode_unsupported_digest_encryption_algorithm" },

		{ pkcs7_format_validator_errc::invalid_signed_data_oid,
			"pe_authenticode_pkcs7_invalid_signed_data_oid" },
		{ pkcs7_format_validator_errc::invalid_signed_data_version,
			"pe_authenticode_pkcs7_invalid_signed_data_version" },
		{ pkcs7_format_validator_errc::invalid_signer_count,
			"pe_authenticode_pkcs7_invalid_signer_count" },
		{ pkcs7_format_validator_errc::non_matching_digest_algorithm,
			"pe_authenticode_pkcs7_non_matching_digest_algorithm" },
		{ pkcs7_format_validator_errc::invalid_signer_info_version,
			"pe_authenticode_pkcs7_invalid_signer_info_version" },
		{ pkcs7_format_validator_errc::absent_message_digest,
			"pe_authenticode_pkcs7_absent_message_digest" },
		{ pkcs7_format_validator_errc::invalid_message_digest,
			"pe_authenticode_pkcs7_invalid_message_digest" },
		{ pkcs7_format_validator_errc::absent_content_type,
			"pe_authenticode_pkcs7_absent_content_type" },
		{ pkcs7_format_validator_errc::invalid_content_type,
			"pe_authenticode_pkcs7_invalid_content_type" },
		{ pkcs7_format_validator_errc::invalid_signing_time,
			"pe_authenticode_pkcs7_invalid_signing_time" },

		{ authenticode_format_validator_errc::invalid_content_info_oid,
			"pe_authenticode_invalid_content_info_oid" },
		{ authenticode_format_validator_errc::invalid_type_value_type,
			"pe_authenticode_invalid_type_value_type" },
		{ authenticode_format_validator_errc::non_matching_type_value_digest_algorithm,
			"pe_authenticode_non_matching_type_value_digest_algorithm" },

		{ authenticode_verifier_errc::invalid_page_hash_format,
			"pe_authenticode_invalid_page_hash_format" },
		{ authenticode_verifier_errc::invalid_image_format_for_hashing,
			"pe_authenticode_invalid_image_format_for_hashing" },
		{ authenticode_verifier_errc::image_security_directory_has_errors,
			"pe_authenticode_image_security_directory_has_errors" },
		{ authenticode_verifier_errc::invalid_authenticode_signature_format,
			"pe_authenticode_invalid_authenticode_signature_format" },

		{ authenticode_timestamp_signature_format_validator_errc::invalid_tst_info_version,
			"pe_authenticode_invalid_tst_info_version" },
		{ authenticode_timestamp_signature_format_validator_errc::invalid_tst_info_accuracy_value,
			"pe_authenticode_invalid_tst_info_accuracy_value" },

		{ signer_info_errc::duplicate_attribute_oid,
			"pe_authenticode_duplicate_attribute_oid" },
		{ signer_info_errc::absent_authenticated_attributes,
			"pe_authenticode_absent_authenticated_attributes" },

		{ certificate_store_errc::absent_certificates,
			"pe_authenticode_absent_certificates" },
		{ certificate_store_errc::duplicate_certificates,
			"pe_authenticode_duplicate_certificates" },

		{ signature_verifier_errc::absent_signing_cert,
			"pe_authenticode_absent_signing_cert" },
		{ signature_verifier_errc::absent_signing_cert_issuer_and_sn,
			"pe_authenticode_absent_signing_cert_issuer_and_sn" },
		{ signature_verifier_errc::unable_to_verify_signature,
			"pe_authenticode_unable_to_verify_signature" },
	};
}

} //namespace

const std::unordered_map<
	std::error_code, std::string_view>& get_authenticode_error_descriptions()
{
	static const auto result = get_ec_to_id_map();
	return result;
}

} //namespace bv::pe
