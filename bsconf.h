/* This file is part of bsconf - a configure replacement.
 *
 * This is the configuration file used by bsconf.c to specify information
 * specific to your project that it needs to substitute into files listed
 * in g_Files. Being a configuration file, this file can be used or
 * modified entirely without restriction. You should change all values
 * appropriately to the name of your project and its requirements. The
 * bsconf license does not apply to this file. It can and should be
 * treated as a template for the creation of your own configuration file.
 *
 * All substituted variable names are given without enclosing @@. For
 * example: "CC" will match "@CC@" in config.h.in and replace it with
 * "gcc" in config.h.
*/

#define PACKAGE_NAME		"fbgl"
#define LIB_MAJOR		"1"
#define LIB_MINOR		"0"
#define LIB_BUILD		"0"

#define PACKAGE_VERSION		LIB_MAJOR "." LIB_MINOR
#define PACKAGE_TARNAME		PACKAGE_NAME
#define PACKAGE_STRING		PACKAGE_NAME " " PACKAGE_VERSION
#define PACKAGE_BUGREPORT	"Mike Sharov <msharov@users.sourceforge.net>"

static cpchar_t g_Files [] = {
    "Common.mk"
};

/* Values substitute @VARNAME@ */
static cpchar_t g_EnvVars [] = {
    "CXX",
    "LD",
    "CPP",
    "HOME",
    "LDFLAGS",
    "CXXFLAGS"
};

/*  VARIABLE	PROGRAM		HOW TO CALL	IF NOT FOUND */
static cpchar_t g_ProgVars [] = {
    "CXX",	"g++",		"g++",		"@CC@",
    "CXX",	"c++",		"c++",		"g++",
    "LD",	"ld",		"ld",		"ld",
    "AR",	"ar",		"ar",		"echo",
    "RANLIB",	"ranlib",	"ranlib",	"touch",
    "INSTALL",	"install",	"install -c",	"cp",
    "RM",	"rm",		"rm -f",	"rm",
    "LN",	"ln",		"ln -sf",	"cp"
};

/*   NAME               IF NOT FOUND                    IF FOUND */
static cpchar_t	g_Headers [] = {
};

/*   NAME               IF NOT FOUND                    IF FOUND */
static cpchar_t g_Libs [] = {
};

/*   NAME               IF NOT FOUND                    IF FOUND */
static cpchar_t g_Functions [] = {
};

/*   NAME               WITHOUT TEXT                            WITH TEXT */
static cpchar_t g_Components [] = {
    "shared",		"#BUILD_SHARED\t= 1",			"BUILD_SHARED\t= 1 ",
    "static",		"#BUILD_STATIC\t= 1",			"BUILD_STATIC\t= 1 ",
    "debug",		"#DEBUG\t\t= 1",			"DEBUG\t\t= 1 ",
    "profile",		"#PROFILE\t= 1",			"PROFILE\t\t= 1 "
};

/* Parallel to g_Components */
static SComponentInfo g_ComponentInfos [VectorSize(g_Components) / 3] = {
    { 1, "Builds the shared library (if supported by the OS)" },
    { 0, "Builds the static library" },
    { 0, "Compiles the library with debugging information" },
    { 0, "Compiles the library with profiling (gprof) information" }
};

/* Substitutes names like @PACKAGE_NAME@ with the second field */
static cpchar_t g_CustomVars [] = {
    "PACKAGE_NAME",		PACKAGE_NAME,
    "PACKAGE_VERSION",		PACKAGE_VERSION,
    "PACKAGE_TARNAME",		PACKAGE_TARNAME,
    "PACKAGE_STRING",		PACKAGE_STRING,
    "PACKAGE_BUGREPORT",	PACKAGE_BUGREPORT,
    "LIBNAME",			PACKAGE_NAME,
    "LIB_MAJOR",		LIB_MAJOR,
    "LIB_MINOR",		LIB_MINOR,
    "LIB_BUILD",		LIB_BUILD
};

/* Maximum size of the subsititution list. bsconf warns on overflow. */
#define MAX_SUBSTITUTIONS	256

