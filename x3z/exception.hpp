#ifndef HAVE_EXCEPTION_T_HPP
#define HAVE_EXCEPTION_T_HPP

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)
#define S__FILE__ S_(__FILE__)
#define STR_FILE __FILE__
#define GENERIC_EXCEPT(type,msg) type(msg": at "__FILE__" "S__LINE__)
#define INVALID_PARAMETER_EXCEPT(x) GENERIC_EXCEPT(invalid_parameter_t, x)
#define OUT_OF_BOUNDS_EXCEPT(x) GENERIC_EXCEPT(out_of_bounds_t, x)
#define API_ERROR_EXCEPT(x) GENERIC_EXCEPT(api_error_t, x)
#define ANNOYED_EXCEPT(x) GENERIC_EXCEPT(annoyed_t, x)

class exception_t : public std::exception
{
	private:
	protected:
		std::string m_msg;

	public:
		explicit exception_t(const char* message) : m_msg(message) {}
		explicit exception_t(const std::string& message) : m_msg(message) {}
		virtual ~exception_t(void) throw( ) {}
		virtual const char* what() const throw( ) { return m_msg.c_str(); }

};

class invalid_parameter_t : public exception_t
{
	public:
		explicit invalid_parameter_t(const char* message) : exception_t("Invalid parameter exception: ")
		{
			m_msg += message;
			return;
		}
		explicit invalid_parameter_t(const std::string& message) : exception_t("Invalid parameter exception: ")
		{
			m_msg += message;
			return;
		}
		~invalid_parameter_t(void) throw( ) { }
};

class out_of_bounds_t : public exception_t
{
	public:
		explicit out_of_bounds_t(const char* message) : exception_t("Out of bounds memory access exception: ")
		{
			m_msg += message;
			return;
		}
		explicit out_of_bounds_t(const std::string& message) : exception_t("Out of bounds memory access exception: ")
		{
			m_msg += message;
			return;
		}
		~out_of_bounds_t(void) throw( ) { }
};

class api_error_t : public exception_t
{
	public:
		explicit api_error_t(const char* message) : exception_t("API access error: ")
		{
			m_msg += message;
			return;
		}
		explicit api_error_t(const std::string& message) : exception_t("API access error: ")
		{
			m_msg += message;
			return;
		}
		~api_error_t(void) throw( ) { }
};

class annoyed_t : public exception_t
{
	public:
	explicit annoyed_t(const char* message) : exception_t("API access error: ")
	{
		m_msg += message;
		return;
	}
	explicit annoyed_t(const std::string& message) : exception_t("API access error: ")
	{
		m_msg += message;
		return;
	}
	~annoyed_t(void) throw( ) { }
};

#endif
