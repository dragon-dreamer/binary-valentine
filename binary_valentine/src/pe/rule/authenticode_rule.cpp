#include "binary_valentine/pe/rule/authenticode_rule.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/pe/rule/authenticode_error_descriptions.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/core/optional_header.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/security/image_authenticode_verifier.h"
#include "pe_bliss2/security/x500/flat_distinguished_name.h"

#include "utilities/variant_helpers.h"

namespace bv::pe
{

namespace
{
constexpr bool is_weak_ecc_curve(pe_bliss::security::pkcs7::ecc_curve curve) noexcept
{
	using enum pe_bliss::security::pkcs7::ecc_curve;
	switch (curve)
	{
	case secp192r1:
	case secp112r1:
	case secp112r2:
	case secp160r1:
	case secp160k1:
	case secp128r1:
	case secp128r2:
	case secp160r2:
	case secp192k1:
	case secp224k1:
	case secp224r1:
	case brainpoolP160r1:
	case brainpoolP192r1:
	case brainpoolP224r1:
		return true;

	default:
		return false;
	}
}

constexpr std::string_view get_ecc_curve_name(pe_bliss::security::pkcs7::ecc_curve curve) noexcept
{
	using enum pe_bliss::security::pkcs7::ecc_curve;
	switch (curve)
	{
	case sm2p256v1: return "sm2p256v1";
	case sm2encrypt_recommendedParameters: return "sm2encrypt";
	case secp192r1: return "secp192r1";
	case secp256r1: return "secp256r1";
	case brainpoolP160r1: return "brainpoolP160r1";
	case brainpoolP192r1: return "brainpoolP192r1";
	case brainpoolP224r1: return "brainpoolP224r1";
	case brainpoolP256r1: return "brainpoolP256r1";
	case brainpoolP320r1: return "brainpoolP320r1";
	case brainpoolP384r1: return "brainpoolP384r1";
	case brainpoolP512r1: return "brainpoolP512r1";
	case secp112r1: return "secp112r1";
	case secp112r2: return "secp112r2";
	case secp160r1: return "secp160r1";
	case secp160k1: return "secp160k1";
	case secp256k1: return "secp256k1";
	case secp128r1: return "secp128r1";
	case secp128r2: return "secp128r2";
	case secp160r2: return "secp160r2";
	case secp192k1: return "secp192k1";
	case secp224k1: return "secp224k1";
	case secp224r1: return "secp224r1";
	case secp384r1: return "secp384r1";
	case secp521r1: return "secp521r1";
	default: return "unknown";
	}
}

constexpr bool is_weak_hash(
	const std::optional<pe_bliss::security::digest_algorithm>& algorithm) noexcept
{
	return algorithm == pe_bliss::security::digest_algorithm::md5
		|| algorithm == pe_bliss::security::digest_algorithm::sha1;
}

constexpr std::string_view get_hash_name(
	const std::optional<pe_bliss::security::digest_algorithm>& algorithm) noexcept
{
	if (!algorithm)
		return "unknown";

	using enum pe_bliss::security::digest_algorithm;
	switch (*algorithm)
	{
	case md5: return "MD5";
	case sha1: return "SHA1";
	case sha256: return "SHA256";
	case sha384: return "SHA384";
	case sha512: return "SHA512";
	default: return "unknown";
	}
}

struct rdn_attribute final
{
	enum value
	{
		common_name = 1 << 0, //CN
		organization = 1 << 1, //O
		locality = 1 << 2, //L
		state = 1 << 3, //S
		country = 1 << 4 //C
	};
};

std::string mask_to_attributes(std::uint32_t mask)
{
	std::string result;
	if (mask & rdn_attribute::common_name)
		result += "CN,";
	if (mask & rdn_attribute::organization)
		result += "O,";
	if (mask & rdn_attribute::locality)
		result += "L,";
	if (mask & rdn_attribute::state)
		result += "S,";
	if (mask & rdn_attribute::country)
		result += "C,";

	if (!result.empty())
		result.pop_back();

	return result;
}
} // namespace

class authenticode_rule final
	: public core::rule_base<authenticode_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_authenticode_rule";
	static constexpr auto reports = output::get_rule_reports<
		authenticode_rule,
		pe_report::signature_check_not_forced,
		pe_report::not_signed,
		pe_report::authenticode_format_error,
		pe_report::authenticode_cert_store_format_error,
		pe_report::authenticode_incorrect_image_hash,
		pe_report::authenticode_incorrect_image_page_hashes,
		pe_report::authenticode_image_page_hashes_absent,
		pe_report::authenticode_image_page_hashes_check_error,
		pe_report::authenticode_incorrect_message_digest,
		pe_report::authenticode_weak_image_hash_algorithm,
		pe_report::authenticode_unable_to_check_signature,
		pe_report::authenticode_weak_rsa_key_size,
		pe_report::authenticode_weak_ecdsa_curve,
		pe_report::authenticode_incorrect_signature,
		pe_report::authenticode_absent_timestamp_signature,
		pe_report::authenticode_incorrect_timestamp_hash,
		pe_report::authenticode_weak_timestamp_digest_algorithm,
		pe_report::authenticode_weak_timestamp_imprint_digest_algorithm,
		pe_report::authenticode_incorrect_timestamp_signature,
		pe_report::authenticode_absent_signing_time,
		pe_report::authenticode_check_error,
		pe_report::authenticode_test_signature,
		pe_report::authenticode_empty_subject_dn,
		pe_report::authenticode_missing_subject_dn_attributes,
		pe_report::authenticode_invalid_subject_dn_attributes>();

	template<typename Reporter>
	void run(Reporter& reporter, const pe_bliss::image::image& image,
		const pe_bliss::security::image_authenticode_check_status* signature) const
	{
		check_integrity(reporter, image, signature != nullptr);
		if (signature != nullptr)
		{
			if (signature->error)
			{
				reporter.template log<pe_report::authenticode_check_error>(
					output::named_arg(output::arg::exception, signature->error));
				return;
			}

			check_signature(reporter, signature->authenticode_status);
		}
	}

private:
	template<typename Reporter>
	static void check_signature(Reporter& reporter,
		const pe_bliss::security::authenticode_check_status<pe_bliss::security::span_range_type>& signature)
	{
		check_signature(reporter, signature.root);
		std::size_t i = 0;
		for (const auto& nested_signature : signature.nested)
			check_signature(reporter, nested_signature, ++i);
	}

	static output::owning_localizable_arg get_signature_info_arg(
		std::optional<std::size_t> nested_signature_index,
		std::string_view root_string_id,
		std::string_view nested_string_id)
	{
		if (nested_signature_index)
		{
			return output::owning_localizable_arg("signature_info", {
				nested_string_id,
				std::array<std::pair<std::string, std::string>, 1u>{
					{
						{ "index", std::to_string(*nested_signature_index) }
					}
			} });
		}
		else
		{
			return output::owning_localizable_arg("signature_info",
				root_string_id);
		}
	}

	template<typename Reporter, typename Signature>
	static bool check_authenticode_format(Reporter& reporter, const Signature& signature,
		const output::owning_localizable_arg& signature_info_arg)
	{
		if (signature.authenticode_format_errors.has_errors())
		{
			error_helpers::report_errors<pe_report::authenticode_format_error>(
				reporter, signature.authenticode_format_errors, get_authenticode_error_descriptions(),
				signature_info_arg);
			return false;
		}

		error_helpers::report_errors<pe_report::authenticode_cert_store_format_error>(
			reporter, signature.certificate_store_warnings, get_authenticode_error_descriptions(),
			signature_info_arg);
		return true;
	}

	template<typename Reporter, typename Signature>
	static void check_message_digest(Reporter& reporter,
		const Signature& signature,
		const output::owning_localizable_arg& signature_info_arg)
	{
		if (signature.message_digest_valid && !*signature.message_digest_valid)
		{
			reporter.template log<pe_report::authenticode_incorrect_message_digest>(
				signature_info_arg);
		}
	}

	template<typename Reporter, typename Signature>
	static void check_signature_result(Reporter& reporter,
		const Signature& signature,
		const output::owning_localizable_arg& signature_info_arg)
	{
		if (!signature.signature_result)
			return;

		if (signature.signature_result->errors.has_errors())
		{
			if (signature.signature_result->processing_error)
			{
				error_helpers::report_errors<pe_report::authenticode_unable_to_check_signature>(
					reporter, signature.signature_result->errors,
					get_authenticode_error_descriptions(), signature_info_arg,
					output::named_arg(output::arg::exception, signature.signature_result->processing_error));
			}
			else
			{
				error_helpers::report_errors<pe_report::authenticode_unable_to_check_signature>(
					reporter, signature.signature_result->errors,
					get_authenticode_error_descriptions(), signature_info_arg,
					output::named_arg(output::arg::exception, output::reports::absent_text));
			}
		}

		const auto& pkcs7_result = signature.signature_result->pkcs7_result;
		if (pkcs7_result.valid) // reported separately if a signature is not valid
		{
			if (signature.digest_encryption_alg
				== pe_bliss::security::digest_encryption_algorithm::rsa)
			{
				static constexpr std::size_t min_rsa_key_size = 2048u;
				if (pkcs7_result.key_size < min_rsa_key_size)
				{
					reporter.template log<pe_report::authenticode_weak_rsa_key_size>(
						signature_info_arg,
						output::named_arg("key_size", pkcs7_result.key_size),
						output::named_arg("min_key_size", min_rsa_key_size));
				}
			}
			else if (signature.digest_encryption_alg
				== pe_bliss::security::digest_encryption_algorithm::ecdsa)
			{
				static constexpr std::size_t min_ecdsa_key_size = 256u;
				if (is_weak_ecc_curve(pkcs7_result.curve))
				{
					reporter.template log<pe_report::authenticode_weak_ecdsa_curve>(
						signature_info_arg,
						output::named_arg("curve", get_ecc_curve_name(pkcs7_result.curve)),
						output::named_arg("min_key_size", min_ecdsa_key_size));
				}
			}
		}
	}

	template<typename Reporter>
	static void check_signature(Reporter& reporter,
		const pe_bliss::security::authenticode_timestamp_signature_check_status<
			pe_bliss::security::span_range_type>& signature,
		std::optional<std::size_t> nested_signature_index)
	{
		const auto signature_info_arg = get_signature_info_arg(nested_signature_index,
			"pe_authenticode_timestamp_signature_info_root",
			"pe_authenticode_timestamp_signature_info_nested");

		if (!check_authenticode_format(reporter, signature, signature_info_arg))
			return;

		if (signature.hash_valid && !*signature.hash_valid)
		{
			reporter.template log<pe_report::authenticode_incorrect_timestamp_hash>(
				signature_info_arg);
		}

		check_message_digest(reporter, signature, signature_info_arg);

		if (is_weak_hash(signature.digest_alg))
		{
			reporter.template log<pe_report::authenticode_weak_timestamp_digest_algorithm>(
				signature_info_arg,
				output::named_arg("hash_algorithm", get_hash_name(signature.digest_alg)));
		}

		if (is_weak_hash(signature.imprint_digest_alg))
		{
			reporter.template log<pe_report::authenticode_weak_timestamp_imprint_digest_algorithm>(
				signature_info_arg,
				output::named_arg("hash_algorithm", get_hash_name(signature.imprint_digest_alg)));
		}

		check_signature_result(reporter, signature, signature_info_arg);

		std::visit(utilities::overloaded{
			[&reporter, &signature_info_arg](std::monostate) {
				reporter.template log<pe_report::authenticode_absent_signing_time>(signature_info_arg);
			},
			[](const auto& /* time */) {}
		}, signature.signing_time);

		if (!signature)
		{
			reporter.template log<pe_report::authenticode_incorrect_timestamp_signature>(
				signature_info_arg);
		}
	}

	template<typename Reporter>
	static void check_signature(Reporter& reporter,
		const pe_bliss::security::authenticode_check_status_base<
			pe_bliss::security::span_range_type>& signature,
		std::optional<std::size_t> nested_signature_index = {})
	{
		const auto signature_info_arg = get_signature_info_arg(nested_signature_index,
			"pe_authenticode_signature_info_root", "pe_authenticode_signature_info_nested");

		if (!check_authenticode_format(reporter, signature, signature_info_arg))
			return;

		if (signature.image_hash_valid && !*signature.image_hash_valid)
		{
			reporter.template log<pe_report::authenticode_incorrect_image_hash>(
				signature_info_arg);
		}

		if (signature.page_hashes_valid && !*signature.page_hashes_valid)
		{
			reporter.template log<pe_report::authenticode_incorrect_image_page_hashes>(
				signature_info_arg);
		}

		if (signature.signature_result && !signature.page_hashes_valid)
		{
			reporter.template log<pe_report::authenticode_image_page_hashes_absent>(
				signature_info_arg);
		}
		
		error_helpers::report_error<pe_report::authenticode_image_page_hashes_check_error>(
			reporter, signature.page_hashes_check_errc, get_authenticode_error_descriptions(),
			signature_info_arg);

		check_message_digest(reporter, signature, signature_info_arg);

		if (is_weak_hash(signature.image_digest_alg))
		{
			reporter.template log<pe_report::authenticode_weak_image_hash_algorithm>(
				signature_info_arg,
				output::named_arg("hash_algorithm", get_hash_name(signature.image_digest_alg)));
		}

		check_signature_result(reporter, signature, signature_info_arg);

		if (!signature.timestamp_signature_result)
			reporter.template log<pe_report::authenticode_absent_timestamp_signature>(signature_info_arg);
		else
			check_signature(reporter, *signature.timestamp_signature_result, nested_signature_index);

		if (signature.signature && signature.cert_store)
		{
			check_signer(reporter, *signature.signature, *signature.cert_store, signature_info_arg);
		}

		if (!signature)
			reporter.template log<pe_report::authenticode_incorrect_signature>(signature_info_arg);
	}

	template<typename Reporter, typename RangeType>
	static void check_signer(Reporter& reporter,
		const pe_bliss::security::authenticode_pkcs7<RangeType>& signature,
		const pe_bliss::security::x509::x509_certificate_store<
			pe_bliss::security::x509::x509_certificate<RangeType>>& cert_store,
		const output::owning_localizable_arg& signature_info_arg)
	{
		if (!signature.get_signer_count())
			return;

		const auto issuer_and_sn = signature.get_signer(0)
			.get_signer_certificate_issuer_and_serial_number();
		const auto* signing_cert = cert_store.find_certificate(
			*issuer_and_sn.serial_number,
			*issuer_and_sn.issuer);

		if (!signing_cert)
			return;

		validate_subject_dn(reporter, signing_cert->get_subject(), signature_info_arg);
	}

	template<typename Reporter, typename RangeType>
	static void validate_subject_dn(Reporter& reporter,
		const pe_bliss::security::x500::flat_distinguished_name<RangeType> subject_dn,
		const output::owning_localizable_arg& signature_info_arg)
	{
		if (subject_dn.empty())
		{
			reporter.template log<pe_report::authenticode_empty_subject_dn>(signature_info_arg);
			return;
		}

		std::uint32_t missing_attributes{}, invalid_attributes{};

		try
		{
			const auto cn = subject_dn.get_common_name();
			if (cn)
			{
				const std::string* cn_str = std::get_if<std::string>(&*cn);
				if (cn_str && cn_str->starts_with("WDKTestCert"))
					reporter.template log<pe_report::authenticode_test_signature>(signature_info_arg);
			}
		}
		catch (const pe_bliss::pe_error&)
		{
			invalid_attributes |= rdn_attribute::common_name;
		}

		auto validate_rdn_attribute = [&missing_attributes,
			&invalid_attributes, &subject_dn](auto attr, auto func) {
			try
			{
				if (!(subject_dn.*func)())
					missing_attributes |= attr;
			}
			catch (const pe_bliss::pe_error&)
			{
				invalid_attributes |= attr;
			}
		};

		using dn_type = pe_bliss::security::x500::flat_distinguished_name<RangeType>;
		validate_rdn_attribute(rdn_attribute::country, &dn_type::get_country_name);
		validate_rdn_attribute(rdn_attribute::locality, &dn_type::get_locality_name);
		validate_rdn_attribute(rdn_attribute::organization, &dn_type::get_organization_name);
		validate_rdn_attribute(rdn_attribute::state, &dn_type::get_state_or_province_name);

		if (missing_attributes)
		{
			reporter.template log<pe_report::authenticode_missing_subject_dn_attributes>(
				signature_info_arg,
				output::named_arg("attributes", mask_to_attributes(missing_attributes)));
		}

		if (invalid_attributes)
		{
			reporter.template log<pe_report::authenticode_invalid_subject_dn_attributes>(
				signature_info_arg,
				output::named_arg("attributes", mask_to_attributes(invalid_attributes)));
		}
	}

	template<typename Reporter>
	static void check_integrity(Reporter& reporter, const pe_bliss::image::image& image,
		bool has_signature)
	{
		if (!has_signature)
		{
			reporter.template log<pe_report::not_signed>();
			return;
		}

		if (!(image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::force_integrity))
		{
			reporter.template log<pe_report::signature_check_not_forced>();
		}
	}
};

void authenticode_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<authenticode_rule>(shared_values);
}

} //namespace bv::pe
