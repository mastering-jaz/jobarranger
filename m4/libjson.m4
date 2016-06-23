##### libjson.html

AC_DEFUN([LIBJSON_CHECK_CONFIG],
[
    AC_ARG_WITH([json],
        AC_HELP_STRING(
            [--with-json@<:@=ARG@:>@],
            [use json-c library @<:@default=yes@:>@, optionally specify the prefix for json-c library]
        ),
        [
            WANT_JSON="yes"
            _json_path="$withval"
        ],
        [WANT_JSON="yes"]
    )

    found_json=no
    JSON_CFLAGS=""
    JSON_LDFLAGS=""
    JSON_LIBS=""
    if test "x$WANT_JSON" = "xyes"; then
	AC_MSG_CHECKING(for JSON-C support)
        for _json_path_tmp in $_json_path /usr /usr/local /opt ; do
           if test -f "$_json_path_tmp/include/json/json.h" && test -r "$_json_path_tmp/include/json/json.h"; then
               if test -f "$_json_path_tmp/lib/libjson.a" && test -r "$_json_path_tmp/lib/libjson.a"; then
                  _json_cppflags="-I$_json_path_tmp/include"
                  _json_ldflags="-L$_json_path_tmp/lib"
                  found_json=yes
                  break;
               fi
            fi
        done

        if test "x$found_json" = "xyes"; then
            JSON_CPPFLAGS="$_json_cppflags"
            JSON_LDFLAGS="$_json_ldflags"
            JSON_LIBS="$_json_path_tmp/lib/libjson.a"

            AC_SUBST(JSON_CPPFLAGS)
            AC_SUBST(JSON_LDFLAGS)
            AC_SUBST(JSON_LIBS)
        fi
    fi
])
