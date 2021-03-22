%{
	/* Demo tokens	*/
	enum token {
		tokint,
		tokdouble,
		tokfloat,
		tokstring,
		tokchar,
		tokarray,
		tokid,
	};
%}

D [0-9]

%%
{D}+	return tokint;
{D}*\.{D}+|{D}+\.{D}*	return tokdouble;
(({D}*\.{D}+)|({D}+\.{D}*))[eE]{D}*		return tokfloat;
'[^\\']'	return tokchar;
\[\]			return tokarray;
[a-zA-Z_][a-z0-9A-Z_]*		return tokid;
%%


