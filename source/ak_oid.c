/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2014 - 2020 by Axel Kenzo, axelkenzo@mail.ru                                     */
/*                                                                                                 */
/*  Файл ak_oid.с                                                                                  */
/*  - содержит реализации функций для работы с идентификаторами криптографических                  */
/*    алгоритмов и параметров                                                                      */
/* ----------------------------------------------------------------------------------------------- */
 #include <ak_parameters.h>

/* ----------------------------------------------------------------------------------------------- */
/*! Константные значения имен идентификаторов */
 static const char *asn1_lcg_n[] =          { "lcg", NULL };
 static const char *asn1_lcg_i[] =          { "1.2.643.2.52.1.1.1", NULL };
#if defined(__unix__) || defined(__APPLE__)
 static const char *asn1_dev_random_n[] =   { "dev-random", "/dev/random", NULL };
 static const char *asn1_dev_random_i[] =   { "1.2.643.2.52.1.1.2", NULL };
 static const char *asn1_dev_urandom_n[] =  { "dev-urandom", "/dev/urandom", NULL };
 static const char *asn1_dev_urandom_i[] =  { "1.2.643.2.52.1.1.3", NULL };
#endif
#ifdef _WIN32
 static const char *asn1_winrtl_n[] =       { "winrtl", NULL };
 static const char *asn1_winrtl_i[] =       { "1.2.643.2.52.1.1.4", NULL };
#endif

 static const char *asn1_w256_pst_n[] =     { "id-tc26-gost-3410-2012-256-paramSetTest", NULL };
 static const char *asn1_w256_pst_i[] =     { "1.2.643.7.1.2.1.1.0",
                                              "1.2.643.2.2.35.0", NULL };
 static const char *asn1_w256_psa_n[] =     { "id-tc26-gost-3410-2012-256-paramSetA", NULL };
 static const char *asn1_w256_psa_i[] =     { "1.2.643.7.1.2.1.1.1", NULL };
 static const char *asn1_w256_psb_n[] =     { "id-tc26-gost-3410-2012-256-paramSetB",
                                              "id-rfc4357-gost-3410-2001-paramSetA",
                                              "id-rfc4357-2001dh-paramSet",
                                              "cspdh",
                                              "cspa", NULL };
 static const char *asn1_w256_psb_i[] =     { "1.2.643.7.1.2.1.1.2",
                                              "1.2.643.2.2.35.1",
                                              "1.2.643.2.2.36.0", NULL };
 static const char *asn1_w256_psc_n[] =     { "id-tc26-gost-3410-2012-256-paramSetC",
                                              "id-rfc4357-gost-3410-2001-paramSetB",
                                              "cspb", NULL };
 static const char *asn1_w256_psc_i[] =     { "1.2.643.7.1.2.1.1.3",
                                              "1.2.643.2.2.35.2", NULL };
 static const char *asn1_w256_psd_n[] =     { "id-tc26-gost-3410-2012-256-paramSetD",
                                              "id-rfc4357-gost-3410-2001-paramSetC",
                                              "cspc", NULL };
 static const char *asn1_w256_psd_i[] =     { "1.2.643.7.1.2.1.1.4",
                                              "1.2.643.2.2.35.3", NULL };
 static const char *asn1_w256_axel_n[] =    { "id-axel-gost-3410-2012-256-paramSetN0",
                                              "axel-n0", NULL };
 static const char *asn1_w256_axel_i[] =    { "1.2.643.2.52.1.12.1.1", NULL };

/* теперь кривые длиной 512 бит */
 static const char *asn1_w512_pst_n[] =     { "id-tc26-gost-3410-2012-512-paramSetTest", NULL };
 static const char *asn1_w512_pst_i[] =     { "1.2.643.7.1.2.1.2.0", NULL };
 static const char *asn1_w512_psa_n[] =     { "id-tc26-gost-3410-2012-512-paramSetA", NULL };
 static const char *asn1_w512_psa_i[] =     { "1.2.643.7.1.2.1.2.1", NULL };
 static const char *asn1_w512_psb_n[] =     { "id-tc26-gost-3410-2012-512-paramSetB", NULL };
 static const char *asn1_w512_psb_i[] =     { "1.2.643.7.1.2.1.2.2", NULL };
 static const char *asn1_w512_psc_n[] =     { "id-tc26-gost-3410-2012-512-paramSetC", NULL };
 static const char *asn1_w512_psc_i[] =     { "1.2.643.7.1.2.1.2.3", NULL };

#ifdef LIBAKRYPT_CRYPTO_FUNCTIONS
 static const char *asn1_akcont_n[] =       { "libakrypt-container", NULL };
 static const char *asn1_akcont_i[] =       { "1.2.643.2.52.1.127.1.1", NULL };
 static const char *asn1_pbkdf2key_n[] =    { "pbkdf2-basic-key", NULL };
 static const char *asn1_pbkdf2key_i[] =    { "1.2.643.2.52.1.127.2.1", NULL };
 static const char *asn1_sdhkey_n[] =       { "static-dh-basic-key", NULL };
 static const char *asn1_sdhkey_i[] =       { "1.2.643.2.52.1.127.2.2", NULL };
 static const char *asn1_extkey_n[] =       { "external-basic-key", NULL };
 static const char *asn1_extkey_i[] =       { "1.2.643.2.52.1.127.2.3", NULL };
 static const char *asn1_symkmd_n[] =       { "symmetric-key-content", NULL };
 static const char *asn1_symkmd_i[] =       { "1.2.643.2.52.1.127.3.1", NULL };
 static const char *asn1_skmd_n[] =         { "secret-key-content", NULL };
 static const char *asn1_skmd_i[] =         { "1.2.643.2.52.1.127.3.2", NULL };
 static const char *asn1_pkmd_n[] =         { "public-key-content", NULL };
 static const char *asn1_pkmd_i[] =         { "1.2.643.2.52.1.127.3.3", NULL };
 static const char *asn1_ecmd_n[] =         { "encrypted-content", NULL };
 static const char *asn1_ecmd_i[] =         { "1.2.643.2.52.1.127.3.4", NULL };
 static const char *asn1_pcmd_n[] =         { "plain-content", NULL };
 static const char *asn1_pcmd_i[] =         { "1.2.643.2.52.1.127.3.5", NULL };

/* добавляем аттрибуты типов (X.500) и расширенные аттрибуты */
 static const char *asn1_email_n[] =        { "email-address", "email", NULL };
 static const char *asn1_email_i[] =        { "1.2.840.113549.1.9.1", NULL };
 static const char *asn1_cn_n[] =           { "common-name", "cn", NULL };
 static const char *asn1_cn_i[] =           { "2.5.4.3", "cn", NULL };
 static const char *asn1_s_n[] =            { "surname", "s", NULL };
 static const char *asn1_s_i[] =            { "2.5.4.4", "s", NULL };
 static const char *asn1_sn_n[] =           { "serial-number", "sn", NULL };
 static const char *asn1_sn_i[] =           { "2.5.4.5", NULL };
 static const char *asn1_c_n[] =            { "country-name", "c", NULL };
 static const char *asn1_c_i[] =            { "2.5.4.6", NULL };
 static const char *asn1_l_n[] =            { "locality-name", "l", NULL };
 static const char *asn1_l_i[] =            { "2.5.4.7", NULL };
 static const char *asn1_st_n[] =           { "state-or-province-name", "st", NULL };
 static const char *asn1_st_i[] =           { "2.5.4.8", NULL };
 static const char *asn1_sa_n[] =           { "street-address", "sa", NULL };
 static const char *asn1_sa_i[] =           { "2.5.4.9", NULL };
 static const char *asn1_o_n[] =            { "organization", "o", NULL };
 static const char *asn1_o_i[] =            { "2.5.4.10", NULL };
 static const char *asn1_ou_n[] =           { "organization-unit", "ou", NULL };
 static const char *asn1_ou_i[] =           { "2.5.4.11", NULL };

 static const char *asn1_ku_n[] =           { "key-usage", NULL };
 static const char *asn1_ku_i[] =           { "2.5.29.15", NULL };
 static const char *asn1_ski_n[] =          { "subject-key-identifier", NULL };
 static const char *asn1_ski_i[] =          { "2.5.29.14", NULL };
 static const char *asn1_bc_n[] =           { "basic-constraints", NULL };
 static const char *asn1_bc_i[] =           { "2.5.29.19", NULL };
 static const char *asn1_cp_n[] =           { "certificate-policies", NULL };
 static const char *asn1_cp_i[] =           { "2.5.29.32", NULL };
 static const char *asn1_wcp_n[] =          { "wildcard-certificate-policy", NULL };
 static const char *asn1_wcp_i[] =          { "2.5.29.32.0", NULL };
 static const char *asn1_aki_n[] =          { "authority-key-identifier", NULL };
 static const char *asn1_aki_i[] =          { "2.5.29.35", NULL };

/* это добро из Приказа ФСБ N 795 */
 static const char *asn1_ogrn_n[] =         { "ogrn", NULL };
 static const char *asn1_ogrn_i[] =         { "1.2.643.100.1", NULL };
 static const char *asn1_snils_n[] =        { "snils", NULL };
 static const char *asn1_snils_i[] =        { "1.2.643.100.3", NULL };
 static const char *asn1_ogrnip_n[] =       { "ogrnip", NULL };
 static const char *asn1_ogrnip_i[] =       { "1.2.643.100.5", NULL };
 static const char *asn1_owner_mod_n[] =    { "subject-crypto-module", NULL };
 static const char *asn1_owner_mod_i[] =    { "1.2.643.100.111", NULL };
 static const char *asn1_issuer_mod_n[] =   { "issuer-crypto-module", NULL };
 static const char *asn1_issuer_mod_i[] =   { "1.2.643.100.112", NULL };
 static const char *asn1_inn_n[] =          { "inn", NULL };
 static const char *asn1_inn_i[] =          { "1.2.643.3.131.1.1", NULL };

 static const char *asn1_class_kc1_n[] =    { "digital-signature-module, class kc1", "kc1", NULL };
 static const char *asn1_class_kc1_i[] =    { "1.2.643.100.113.1", NULL };
 static const char *asn1_class_kc2_n[] =    { "digital-signature-module, class kc2", "kc2", NULL };
 static const char *asn1_class_kc2_i[] =    { "1.2.643.100.113.2", NULL };
 static const char *asn1_class_kc3_n[] =    { "digital-signature-module, class kc3", "kc3", NULL };
 static const char *asn1_class_kc3_i[] =    { "1.2.643.100.113.3", NULL };
 static const char *asn1_class_kb1_n[] =    { "digital-signature-module, class kb1", "kb", NULL };
 static const char *asn1_class_kb1_i[] =    { "1.2.643.100.113.4", NULL };
 static const char *asn1_class_kb2_n[] =    { "digital-signature-module, class kb2", NULL };
 static const char *asn1_class_kb2_i[] =    { "1.2.643.100.113.5", NULL };
 static const char *asn1_class_ka1_n[] =    { "digital-signature-module, class ka", "ka", NULL };
 static const char *asn1_class_ka1_i[] =    { "1.2.643.100.113.6", NULL };

/* ----------------------------------------------------------------------------------------------- */
/* вот что приходится разбирать в сертификатах от КриптоПро */
/*   Microsoft OID...................................1.3.6.1.4.1.311  */
/* ----------------------------------------------------------------------------------------------- */
 static const char *asn1_mscav_n[] =        { "microsoft-ca-version", NULL };
 static const char *asn1_mscav_i[] =        { "1.3.6.1.4.1.311.21.1", NULL };
 static const char *asn1_mspsh_n[] =        { "microsoft-previous-certhash", NULL };
 static const char *asn1_mspsh_i[] =        { "1.3.6.1.4.1.311.21.2", NULL };
#endif

/* ----------------------------------------------------------------------------------------------- */
/*! Константные значения OID библиотеки */
static struct oid libakrypt_oids[] =
{
 /* идентификаторы  */
 {{ random_generator, algorithm, asn1_lcg_i, asn1_lcg_n }, NULL,
  { sizeof( struct random ), (ak_function_create_object *)ak_random_context_create_lcg,
                                   (ak_function_destroy_object *)ak_random_context_destroy }},
#if defined(__unix__) || defined(__APPLE__)
 {{ random_generator, algorithm, asn1_dev_random_i, asn1_dev_random_n }, NULL,
  { sizeof( struct random ), (ak_function_create_object *)ak_random_context_create_random,
                                   (ak_function_destroy_object *)ak_random_context_destroy }},
 {{ random_generator, algorithm, asn1_dev_urandom_i, asn1_dev_urandom_n }, NULL,
  { sizeof( struct random ), (ak_function_create_object *)ak_random_context_create_urandom,
                                   (ak_function_destroy_object *)ak_random_context_destroy }},
#endif
#ifdef _WIN32
 {{ random_generator, algorithm, asn1_winrtl_i, asn1_winrtl_n }, NULL,
  { sizeof( struct random ), (ak_function_create_object *)ak_random_context_create_winrtl,
                                   (ak_function_destroy_object *)ak_random_context_destroy }},
#endif

 {{ identifier, wcurve_params, asn1_w256_pst_i, asn1_w256_pst_n },
                 (ak_pointer) &id_tc26_gost_3410_2012_256_paramSetTest, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w256_psa_i, asn1_w256_psa_n },
                    (ak_pointer) &id_tc26_gost_3410_2012_256_paramSetA, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w256_psb_i, asn1_w256_psb_n },
                     (ak_pointer) &id_rfc4357_gost_3410_2001_paramSetA, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w256_psc_i, asn1_w256_psc_n },
                     (ak_pointer) &id_rfc4357_gost_3410_2001_paramSetB, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w256_psd_i, asn1_w256_psd_n },
                     (ak_pointer) &id_rfc4357_gost_3410_2001_paramSetC, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w256_axel_i, asn1_w256_axel_n },
                  (ak_pointer) &id_axel_gost_3410_2012_256_paramSet_N0, ak_object_undefined },

 {{ identifier, wcurve_params, asn1_w512_pst_i, asn1_w512_pst_n },
                  (ak_pointer) &id_tc26_gost_3410_2012_512_paramSetTest, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w512_psa_i, asn1_w512_psa_n },
                  (ak_pointer) &id_tc26_gost_3410_2012_512_paramSetA, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w512_psb_i, asn1_w512_psb_n },
                  (ak_pointer) &id_tc26_gost_3410_2012_512_paramSetB, ak_object_undefined },
 {{ identifier, wcurve_params, asn1_w512_psc_i, asn1_w512_psc_n },
                  (ak_pointer) &id_tc26_gost_3410_2012_512_paramSetC, ak_object_undefined },

#ifdef LIBAKRYPT_CRYPTO_FUNCTIONS
 {{ identifier, descriptor, asn1_akcont_i, asn1_akcont_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_pbkdf2key_i, asn1_pbkdf2key_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_sdhkey_i, asn1_sdhkey_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_extkey_i, asn1_extkey_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_symkmd_i, asn1_symkmd_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_skmd_i, asn1_skmd_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_pkmd_i, asn1_pkmd_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_ecmd_i, asn1_ecmd_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_pcmd_i, asn1_pcmd_n }, NULL, ak_object_undefined },

 {{ identifier, descriptor, asn1_email_i, asn1_email_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_cn_i, asn1_cn_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_s_i, asn1_s_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_sn_i, asn1_sn_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_c_i, asn1_c_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_l_i, asn1_l_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_st_i, asn1_st_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_sa_i, asn1_sa_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_o_i, asn1_o_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_ou_i, asn1_ou_n }, NULL, ak_object_undefined },

 {{ identifier, descriptor, asn1_ku_i, asn1_ku_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_ski_i, asn1_ski_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_bc_i, asn1_bc_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_cp_i, asn1_cp_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_wcp_i, asn1_wcp_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_aki_i, asn1_aki_n }, NULL, ak_object_undefined },

 {{ identifier, descriptor, asn1_ogrn_i, asn1_ogrn_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_snils_i, asn1_snils_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_ogrnip_i, asn1_ogrnip_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_owner_mod_i, asn1_owner_mod_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_issuer_mod_i, asn1_issuer_mod_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_inn_i, asn1_inn_n }, NULL, ak_object_undefined },

 {{ identifier, descriptor, asn1_class_kc1_i, asn1_class_kc1_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_class_kc2_i, asn1_class_kc2_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_class_kc3_i, asn1_class_kc3_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_class_kb1_i, asn1_class_kb1_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_class_kb2_i, asn1_class_kb2_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_class_ka1_i, asn1_class_ka1_n }, NULL, ak_object_undefined },
 
 {{ identifier, descriptor, asn1_mscav_i, asn1_mscav_n }, NULL, ak_object_undefined },
 {{ identifier, descriptor, asn1_mspsh_i, asn1_mspsh_n }, NULL, ak_object_undefined },
#endif 

 /* завершающая константа, должна всегда принимать неопределенные и нулевые значения */
 { ak_oid_info_undefined, NULL, ak_object_undefined }
};

/* ----------------------------------------------------------------------------------------------- */
static const char *libakrypt_engine_names[] = {
    "identifier",
    "block cipher",
    "stream cipher",
    "hybrid cipher",
    "hash function",
    "hmac function",
    "cmac function",
    "mgm function",
    "mac function",
    "sign function",
    "verify function",
    "random generator",
    "oid engine",
    "undefined engine",
};

/* ----------------------------------------------------------------------------------------------- */
static const char *libakrypt_mode_names[] = {
    "algorithm",
    "parameter",
    "wcurve params",
    "ecurve params",
    "kbox params",
    "ecb",
    "counter",
    "counter_gost",
    "ofb",
    "cbc",
    "cfb",
    "xts",
    "xtsmac",
    "xcrypt",
    "a8",
    "descriptor",
    "undefined mode"
};

/* ----------------------------------------------------------------------------------------------- */
/*                     реализация функций доступа к глобальному списку OID                         */
/* ----------------------------------------------------------------------------------------------- */
/*! \addtogroup frontend_oid
   Блалала                                                                                         */
/* ----------------------------------------------------------------------------------------------- */
 size_t ak_libakrypt_oids_count( void )
{
 return ( sizeof( libakrypt_oids )/( sizeof( struct oid )) - 1 );
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param engine Тип криптографического механизма.
    \return Функция возвращает указатель на константную строку.                                    */
/* ----------------------------------------------------------------------------------------------- */
 const char *ak_libakrypt_get_engine_name( const oid_engines_t engine )
{
  if( engine > undefined_engine ) {
    ak_error_message_fmt( ak_error_oid_engine, __func__, "incorrect value of engine: %d", engine );
    return ak_null_string;
  }
 return libakrypt_engine_names[engine];
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param mode Режим криптографического механизма.
    \return Функция возвращает указатель на константную строку.                                    */
/* ----------------------------------------------------------------------------------------------- */
 const char *ak_libakrypt_get_mode_name( const oid_modes_t mode )
{
  if( mode > undefined_mode ) {
    ak_error_message_fmt( ak_error_oid_mode, __func__, "incorrect value of engine mode: %d", mode );
    return ak_null_string;
  }
 return libakrypt_mode_names[mode];
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param index Индекс статической структуры oid.
    \param info Указатель на контекст, куда будет помещена информация о криптографическом механизме;
    перед вызовом функции, объект `info` должен быть размещен в статической или динамической памяти.

    \return Функция возвращает \ref ak_error_ok (ноль) в случае успеха. В противном случае,
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 dll_export int ak_libakrypt_get_oid_by_index( const size_t index, ak_oid_info info )
{
 /* проверяем, что мы нужном диапазоне */
  if( index >= ak_libakrypt_oids_count())
    return ak_error_message( ak_error_wrong_index, __func__, "incorrect index value" );
  if( info == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                        "using null pointer to oid_info context" );
 /* в случае успеха, возвращаем запрашиваемую информацию */
  info->engine = libakrypt_oids[index].info.engine;
  info->mode = libakrypt_oids[index].info.mode;
  info->id =  libakrypt_oids[index].info.id; 
  info->name = libakrypt_oids[index].info.name;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param description Строка, содержащая имя или идентификатор криптографического алгоритма.
    \param info Указатель на контекст, куда будет помещена информация о криптографическом механизме;
    перед вызовом функции, объект `oid` должен быть размещен в статической или динамической памяти.

    \return Функция возвращает \ref ak_error_ok (ноль) в случае успеха. В противном случае,
    возвращается код ошибки.                                                                       */
/* ----------------------------------------------------------------------------------------------- */
 dll_export int ak_libakrypt_get_oid( const char *description , ak_oid_info info )
{
  ak_oid oid = NULL;

 /* проверяем область допустимых значений */
  if( description == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                   "using null pointer to string with name/identifer of cryptographic mechanism" );
  if( info == NULL ) return ak_error_message( ak_error_null_pointer, __func__,
                                                        "using null pointer to oid_info context" );
  if(( oid = ak_oid_context_find_by_ni( description )) == NULL )
    return ak_error_message( ak_error_wrong_oid, __func__,
                               "incorrect string with name/identifer of cryptographic mechanism" );
 /* в случае успеха, возвращаем запрашиваемую информацию */
  info->engine = oid->info.engine;
  info->mode = oid->info.mode;
  info->id =  oid->info.id;
  info->name = oid->info.name;

 return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*                           функции для создания объектов по oid                                  */
/* ----------------------------------------------------------------------------------------------- */
/*! \param oid Идентификатор создаваемого объекта
    \return Функция возвращает указатель на контекст созданного объекта. В случае возникновения
    ошибки возвращается NULL. */
/* ----------------------------------------------------------------------------------------------- */
ak_pointer ak_oid_context_new_object( ak_oid oid )
{
  ak_pointer ctx = NULL;
  int error = ak_error_ok;
  
  if( oid == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__,
		      "use a null pointer to object identifer" );
    return NULL;
  }
  if( oid->func.create == NULL ) {
    ak_error_message( ak_error_undefined_function, __func__,
		      "create an object that does not support this feature" );
    return NULL;
  }
  
  if(( ctx = malloc( oid->func.size )) != NULL ) {
    if(( error = ((ak_function_create_object*)oid->func.create )( ctx )) != ak_error_ok ) {
      ak_error_message_fmt( error, __func__, "the creation of the %s object failed",
			    ak_libakrypt_get_engine_name( oid->info.engine ));
      if( ctx != NULL ) {
	free( ctx );
        ctx = NULL;
      }
    }	
  } else
    ak_error_message( ak_error_out_of_memory, __func__, "memory allocation error" );
  
 return ctx; 
}

/* ----------------------------------------------------------------------------------------------- */
/*! \param oid Идентификатор удаляемого объекта
    \param ctx Контекст удаляемого объекта
    \return Функция всегда возвращает NULL.                                                        */
/* ----------------------------------------------------------------------------------------------- */
ak_pointer ak_oid_context_delete_object( ak_oid oid, ak_pointer ctx )
{
  int error = ak_error_ok;

  if( ctx == NULL ) return ctx;
  if( oid == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__,
		      "use a null pointer to object identifer" );
    return NULL;
  }
  if( oid->func.create == NULL ) {
    ak_error_message( ak_error_undefined_function, __func__,
		      "destroy an object that does not support this feature" );
    return NULL;
  }

  if(( error = ((ak_function_destroy_object*)oid->func.destroy )( ctx )) != ak_error_ok )
      ak_error_message_fmt( error, __func__, "the destroing of %s object failed",
			    ak_libakrypt_get_engine_name( oid->info.engine ));
  
  if( ctx != NULL ) free( ctx );

return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*                          поиск OID - функции внутреннего интерфейса                             */
/* ----------------------------------------------------------------------------------------------- */
/*! @param name строка, содержащая символьное (человекочитаемое) имя криптографического механизма
    или параметра.
    @return Функция возвращает указатель на область памяти, в которой находится структура
    с найденным идентификатором. В случае ошибки, возвращается NULL и устанавливается код ошибки.  */
/* ----------------------------------------------------------------------------------------------- */
 ak_oid ak_oid_context_find_by_name( const char *name )
{
  size_t idx = 0;

 /* надо ли стартовать */
  if( name == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__, "using null pointer to oid name" );
    return NULL;
  }
 /* перебор по всем возможным значениям */
  do{
     const char *str = NULL;
     size_t len = 0, jdx = 0;
     while(( str = libakrypt_oids[idx].info.name[jdx] ) != NULL ) {
        len = strlen( str );
        if(( strlen( name ) == len ) && ak_ptr_is_equal( name, str, len ))
          return  &libakrypt_oids[idx];
        jdx++;
     }
  } while( ++idx < ak_libakrypt_oids_count( ));

  ak_error_set_value( ak_error_oid_id );
 return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param id строка, содержащая символьную запись идентификатора - последовательность чисел,
    разделенных точками.
    @return Функция возвращает указатель на область памяти, в которой находится структура
    с найденным идентификатором. В случае ошибки, возвращается NULL.                               */
/* ----------------------------------------------------------------------------------------------- */
 ak_oid ak_oid_context_find_by_id( const char *id )
{
  size_t idx = 0;
  if( id == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__, "using null pointer to oid identifier" );
    return NULL;
  }
 /* перебор по всем возможным значениям */
  do{
     const char *str = NULL;
     size_t len = 0, jdx = 0;
     while(( str = libakrypt_oids[idx].info.id[jdx] ) != NULL ) {
        len = strlen( str );
        if(( strlen( id ) == len ) && ak_ptr_is_equal( id, str, len ))
          return  &libakrypt_oids[idx];
        jdx++;
     }
  } while( ++idx < ak_libakrypt_oids_count( ));

  ak_error_set_value( ak_error_oid_id );
 return NULL;
}


/* ----------------------------------------------------------------------------------------------- */
/*! @param ni строка, содержащая символьную запись имени или идентификатора - последовательности
    чисел, разделенных точками.
    @return Функция возвращает указатель на область памяти, в которой находится структура
    с найденным идентификатором. В случае ошибки, возвращается NULL.                               */
/* ----------------------------------------------------------------------------------------------- */
 ak_oid ak_oid_context_find_by_ni( const char *ni )
{
  size_t idx = 0;
  if( ni == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__,
		      "using null pointer to oid name or identifier" );
    return NULL;
  }

  /* перебор по всем возможным значениям имен */
  do{
    const char *str = NULL;
    size_t len = 0, jdx = 0;
    while(( str = libakrypt_oids[idx].info.name[jdx] ) != NULL ) {
      len = strlen( str );
      if(( strlen( ni ) == len ) && ak_ptr_is_equal( ni, str, len ))
        return  &libakrypt_oids[idx];
      jdx++;
    }
  } while( ++idx < ak_libakrypt_oids_count( ));

  /* перебор по всем возможным значениям идентификаторов */
  idx = 0;
  do{
    const char *str = NULL;
    size_t len = 0, jdx = 0;
    while(( str = libakrypt_oids[idx].info.id[jdx] ) != NULL ) {
      len = strlen( str );
      if(( strlen( ni ) == len ) && ak_ptr_is_equal( ni, str, len ))
        return  &libakrypt_oids[idx];
      jdx++;
    }
  } while( ++idx < ak_libakrypt_oids_count( ));

  ak_error_set_value( ak_error_oid_id );
 return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param ptr указатель а область памяти, по которой ищется oid
    @return Функция возвращает указатель на область памяти, в которой находится структура
    с найденным идентификатором. В случае ошибки, возвращается NULL.                               */
/* ----------------------------------------------------------------------------------------------- */
 ak_oid ak_oid_context_find_by_data( ak_const_pointer ptr )
{
  size_t idx = 0;

 /* надо ли стартовать */
  if( ptr == NULL ) {
    ak_error_message( ak_error_null_pointer, __func__, "using null pointer to oid name" );
    return NULL;
  }

 /* перебор по всем возможным значениям */
  do{
     if( libakrypt_oids[idx].data == ptr ) return  &libakrypt_oids[idx];
  } while( ++idx < ak_libakrypt_oids_count( ));

  ak_error_set_value( ak_error_oid_id );
 return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param engine тип криптографическиого механизма.

    @return В случае успешного поиска функция возвращает указатель на  область памяти, в которой
    находится структура с найденным идентификатором. В случае ошибки, возвращается NULL.           */
/* ----------------------------------------------------------------------------------------------- */
 ak_oid ak_oid_context_find_by_engine( const oid_engines_t engine )
{
  size_t idx = 0;
  do{
     if( libakrypt_oids[idx].info.engine == engine ) return &libakrypt_oids[idx];
  } while( ++idx < ak_libakrypt_oids_count( ));
  ak_error_message( ak_error_oid_name, __func__, "searching oid with wrong engine" );

 return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param startoid предыдущий найденный oid.
    @param engine тип криптографическиого механизма.

    @return В случае успешного поиска функция возвращает указатель на  область памяти, в которой
    находится структура с найденным идентификатором. В случае ошибки, возвращается NULL.           */
/* ----------------------------------------------------------------------------------------------- */
 ak_oid ak_oid_context_findnext_by_engine( const ak_oid startoid, const oid_engines_t engine )
{
 ak_oid oid = ( ak_oid )startoid;

 if( oid == NULL) {
   ak_error_message( ak_error_null_pointer, __func__, "using null pointer to oid" );
   return NULL;
 }

 /* сдвигаемся по массиву OID вперед */
  while( (++oid)->info.engine != undefined_engine ) {
    if( oid->info.engine == engine ) return oid;
  }

 return NULL;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param oid Тестируемый на корректность адрес
    @return Функция возвращает истину, если заданный адрес `oid` дествительности содержится
    среди предопределенных oid библиотеки.                                                         */
/* ----------------------------------------------------------------------------------------------- */
 bool_t ak_oid_context_check( const ak_oid oid )
{
  size_t i;
  bool_t result = ak_false;

  for( i = 0; i < ak_libakrypt_oids_count(); i++ )
     if( oid == &libakrypt_oids[i] ) result = ak_true;

 return result;
}

/* ----------------------------------------------------------------------------------------------- */
/*!  \example test-oid01.c                                                                         */
/*!  \example test-oid02.c                                                                         */
/*!  \example example-oid.c                                                                        */
/* ----------------------------------------------------------------------------------------------- */
/*                                                                                       ak_oid.c  */
/* ----------------------------------------------------------------------------------------------- */
