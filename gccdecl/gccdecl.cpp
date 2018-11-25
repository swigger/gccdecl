/*
 #define DEFTREECODE(SYM, STRING, TYPE, NARGS)   {SYM, #SYM, STRING},
 #define END_OF_BASE_TREE_CODES {LAST_AND_UNUSED_TREE_CODE, 0, 0},
 struct tree_info_t {
 int val;
 const char * id;
 const char * str;
 } tree_info[] = {
 #include "all-tree.def"
 {0,0,0}
 };
 #undef DEFTREECODE
 
 ref: https://github.com/ruediger/gcc-etags.git
 // g++ -I`$(g++) -print-file-name=plugin`/include -fPIC
 */

#include <stdlib.h>
#include <gmp.h>
#include <cstdlib>
#include <string>
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <stdint.h>


const char * dump_hex(uintptr_t stpos, const void * buf, size_t sz, std::string & sa)
{
	size_t osz = sa.length();
	if (!buf)
	{
		sa += "(null)\r\n";
		return sa.c_str()+osz;
	}
	
	const char * hextbl = "0123456789abcdef";
	char cbuf[100];
	
	for (size_t i=0; i<sz; i+=16)
	{
		const char * ptr = (const char *)buf + i;
		uintptr_t addr = stpos + i;
		size_t l = sz-i; if (l>16) l = 16;
		char * os = cbuf;
		
		for (int j=0; j<8; ++j)
		{
			int o = 4 * (8-j-1);
			*os++ = hextbl[0xf & (addr>>o)];
		}
		*os++ = ':';
		*os++ = ' ';
		*os++ = ' ';
		
		for (uint32_t i=0; i<16; ++i)
		{
			if (i<l)
			{
				*os++ = hextbl[0xf&(ptr[i]>>4)];
				*os++ = hextbl[0xf&(ptr[i])];
				*os++ = (i==7&&l>8) ? '-' : ' ';
			}
			else
			{
				*os ++ = ' ';
				*os ++ = ' ';
				*os ++ = ' ';
			}
		}
		*os++ = ' ';
		*os++ = ' ';
		*os++ = ' ';
		
		for (uint32_t i=0; i<l; ++i)
		{
			unsigned char ch = ptr[i];
			*os++ = isprint(ch) ? ch : '.';
		}
		*os ++ = '\r';
		*os ++ = '\n';
		*os ++ = 0;
		sa += cbuf;
	}
	return sa.c_str()+osz;
}
size_t dump_hex(uintptr_t spos, const void * buf, size_t sz, FILE * fo)
{
	std::string s;
	dump_hex(spos, buf, sz, s);
	fwrite(s.data(), 1, s.length(), fo);
	return s.length();
}

#include "gcc-plugin.h"
#include "plugin-version.h"
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "intl.h"
#include "tm.h"
#include "cp/cp-tree.h"
#include "c-family/c-common.h"
#include "c-family/c-pragma.h"
#include "diagnostic.h"

int plugin_is_GPL_compatible;

using std::string;
using std::cerr;
using std::cout;

const char * decl_name_p(tree decl)
{
	tree name = DECL_NAME(decl);
	return name ? IDENTIFIER_POINTER(name) : "";
}

string tree_type(tree t, string & postfix)
{
	string rr,tmp;
	if(!t) return rr;
	tree_code tc = TREE_CODE(t);
	switch (tc) {
	case OFFSET_TYPE:
	case BOOLEAN_TYPE:
	case INTEGER_TYPE:
	case REAL_TYPE:
	case ENUMERAL_TYPE:
	case FUNCTION_TYPE:
	case RECORD_TYPE:
	case POINTER_TYPE:
		if (t->type_common.name && TREE_CODE(t->type_common.name) == TYPE_DECL &&
			t->type_common.name->decl_minimal.name &&
			TREE_CODE(t->type_common.name->decl_minimal.name) == IDENTIFIER_NODE)
		{
			rr = IDENTIFIER_POINTER(t->type_common.name->decl_minimal.name);
			if (t->type_common.common.typed.base.readonly_flag ||
				t->type_common.common.typed.base.constant_flag)
				rr += " const";
			if (t->type_common.common.typed.base.volatile_flag)
				rr += " volatile";
			return rr;
		}
		break;
	default:
		break;
	}
	
	switch (tc)
	{
	case IDENTIFIER_NODE:
		return IDENTIFIER_POINTER(t);
	case VOID_TYPE:
		return "void";
	case INTEGER_TYPE:
	case REAL_TYPE:
		rr = IDENTIFIER_POINTER(DECL_NAME(t->type_common.name));
		if (t->type_common.common.typed.base.readonly_flag ||
			t->type_common.common.typed.base.constant_flag)
			rr += " const";
		if (t->type_common.common.typed.base.volatile_flag)
			rr += " volatile";
		return rr;
	case POINTER_TYPE:
		rr = tree_type(t->typed.type, tmp);
		rr += "*";
		if (t->type_common.common.typed.base.readonly_flag ||
			t->type_common.common.typed.base.constant_flag)
			rr += " const";
		if (t->type_common.common.typed.base.volatile_flag)
			rr += " volatile";
		postfix += tmp;
		return rr;
	case TYPE_DECL:
		return decl_name_p(t);
	case RECORD_TYPE:
		rr = "struct " + tree_type(TYPE_NAME(t), tmp);
		assert(tmp.empty());
		return rr;
	case PARM_DECL:
		for (tree decl = t; decl != 0; decl = TREE_CHAIN (decl)) {
			string tp = tree_type(decl->typed.type, tmp);
			string na = decl_name_p(decl);
			rr += tp;
			rr += " ";
			rr += na;
			rr += tmp;
			rr += ", ";
			tmp.clear();
		}
		if (rr.size() > 2)
			rr.resize(rr.size()-2);
		return rr;
	case FUNCTION_TYPE:
		{
			string rt = tree_type(t->typed.type, tmp);
			rt += tmp; tmp.clear();
			string args;
			for (tree x = t->type_non_common.values; x && x->list.value; x = x->list.common.chain)
			{
				if (TREE_CODE(x->list.value) == VOID_TYPE)
					break;
				string aa = tree_type(x->list.value, tmp);
				args += aa;
				args += tmp;
				args += ", ";
				tmp.clear();
				if (x->list.common.chain == 0)
				{
					args += "..., ";
					break;
				}
			}
			if (args.size()>2) args.resize(args.size()-2);
			assert(postfix.empty());
			postfix = ")("; postfix+=args; postfix += ")";
			rt += "(";
			return rt;
		}
		return "???";
	default:
		sleep(0);
		return "???";
	}
}

void plugin_finish_decl (void *event_data, void *data)
{
	tree decl = (tree) event_data;
	if (! DECL_FILE_SCOPE_P(decl)) return;

	if (TREE_CODE(decl) == FUNCTION_DECL)
	{
		static FILE * flog = 0;
		if (flog == 0) flog = fopen("/tmp/funcs.txt", "a+");
#ifdef DEBUG
		flog = stderr;
#endif

		const char * funcname = IDENTIFIER_POINTER (DECL_NAME (decl));
		//dump_hex(0, decl, sizeof(tree_node), stderr);
		string pre,post,func;
		pre = tree_type(decl->typed.type, post);
		if (!pre.empty() && !post.empty())
		{
			pre.resize(pre.size()-1);
			post = post.substr(1);
			func = pre + " " + funcname + post;
			bool ext = TREE_PUBLIC(decl);
			if (ext)
			{
				fprintf(flog, "%s;\n", func.c_str());
				//warning (0, "Decl function:\n %s ", ext?"extern":"static", );
			}
		}
	}
}

int plugin_init(plugin_name_args *info, plugin_gcc_version *version)
{
	if (not plugin_default_version_check(version, &gcc_version))
	{
		cerr << "ERROR: failed version check!\n";
		return 1;
	}
	//char cmd[1000];
	//sprintf(cmd, "ps aux | grep %d", (int)getpid());
	//system(cmd);
	
	register_callback(info->base_name,  PLUGIN_FINISH_DECL, &plugin_finish_decl, 0);
	return 0;
}
