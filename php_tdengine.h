/* tdengine extension for PHP */

#ifndef PHP_TDENGINE_H
# define PHP_TDENGINE_H

extern zend_module_entry tdengine_module_entry;
# define phpext_tdengine_ptr &tdengine_module_entry

# define PHP_TDENGINE_VERSION "1.0.5"

# if defined(ZTS) && defined(COMPILE_DL_TDENGINE)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_TDENGINE_H */
