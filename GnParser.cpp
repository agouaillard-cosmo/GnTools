

// This file was automatically generated by Coco/R; don't modify it.
#include "GnParser.h"
#include "GnErrors.h"
#include <QtDebug>
#include <QFileInfo>

namespace Gn {


static QString coco_string_create( const wchar_t* str )
{
    return QString::fromStdWString(str);
}

void Parser::ParsePrimaryExpr()
{
    d_stack.push(&d_root);
	d_cur = PARSER_NS::Token();
	d_next = PARSER_NS::Token();
	Get();
	PrimaryExpr();
	Expect(0,__FUNCTION__);
    d_stack.pop();
}

int Parser::peek( quint8 la )
{
	if( la == 0 )
		return d_cur.d_type;
	else if( la == 1 )
		return d_next.d_type;
	else
		return scanner->peekToken( la - 1 ).d_type;
}


void Parser::SynErr(int n, const char* ctx) {
    if (errDist >= minErrDist)
    {
       SynErr(d_next.d_sourcePath,d_next.d_lineNr, d_next.d_colNr, n, errors, ctx);
    }
	errDist = 0;
}

void Parser::SemErr(const char* msg) {
	if (errDist >= minErrDist) errors->error(PARSER_NS::Errors::Semantics,d_cur.d_sourcePath,d_cur.d_lineNr, d_cur.d_colNr, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		d_cur = d_next;
		d_next = scanner->nextToken();
        bool deliverToParser = false;
        switch( d_next.d_type )
        {
        case PARSER_NS::Tok_Invalid:
        	if( !d_next.d_val.isEmpty() )
            	SynErr( d_next.d_type, d_next.d_val );
            // else errors already handeled in lexer
            break;
        case PARSER_NS::Tok_Comment:
            d_comments.append(d_next);
            break;
        default:
            deliverToParser = true;
            break;
        }

        if( deliverToParser )
        {
            if( d_next.d_type == PARSER_NS::Tok_Eof )
                d_next.d_type = _EOF;

            la->kind = d_next.d_type;
            if (la->kind <= maxT)
            {
                ++errDist;
                break;
            }
        }

		d_next = d_cur;
	}
}

void Parser::Expect(int n, const char* ctx ) {
	if (la->kind==n) Get(); else { SynErr(n, ctx); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::File() {
		d_stack.push(&d_root); 
		StatementList();
		d_stack.pop(); 
}

void Parser::StatementList() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_StatementList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		while (la->kind == _T_if || la->kind == _T_identifier) {
			Statement();
		}
		d_stack.pop(); 
}

void Parser::Statement() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Statement, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (peek(1) == _T_identifier && peek(2) == _T_Lpar ) {
			Call();
		} else if (la->kind == _T_identifier) {
			Assignment();
		} else if (la->kind == _T_if) {
			Condition();
		} else SynErr(38,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::Call() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Call, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_identifier,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Lpar,__FUNCTION__);
		addTerminal(); 
		if (StartOf(1)) {
			ExprList();
		}
		Expect(_T_Rpar,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_Lbrace) {
			Block();
		}
		d_stack.pop(); 
}

void Parser::Assignment() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Assignment, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		LValue();
		AssignOp();
		Expr();
		d_stack.pop(); 
}

void Parser::Condition() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Condition, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_if,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Lpar,__FUNCTION__);
		addTerminal(); 
		Expr();
		Expect(_T_Rpar,__FUNCTION__);
		addTerminal(); 
		Block();
		if (la->kind == _T_else) {
			Get();
			addTerminal(); 
			if (la->kind == _T_if) {
				Condition();
			} else if (la->kind == _T_Lbrace) {
				Block();
			} else SynErr(39,__FUNCTION__);
		}
		d_stack.pop(); 
}

void Parser::LValue() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_LValue, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_identifier,__FUNCTION__);
		addTerminal(); 
		if (la->kind == _T_Dot || la->kind == _T_Lbrack) {
			if (la->kind == _T_Lbrack) {
				Get();
				addTerminal(); 
				Expr();
				Expect(_T_Rbrack,__FUNCTION__);
				addTerminal(); 
			} else {
				Get();
				addTerminal(); 
				Expect(_T_identifier,__FUNCTION__);
				addTerminal(); 
			}
		}
		d_stack.pop(); 
}

void Parser::Expr() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Expr, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		UnaryExpr();
		if (StartOf(2)) {
			Expr_nlr_();
		}
		d_stack.pop(); 
}

void Parser::AssignOp() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_AssignOp, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_Eq) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_PlusEq) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_MinusEq) {
			Get();
			addTerminal(); 
		} else SynErr(40,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::ExprList() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_ExprList, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expr();
		while (la->kind == _T_Comma) {
			Get();
			addTerminal(); 
			Expr();
		}
		d_stack.pop(); 
}

void Parser::Block() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Block, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_Lbrace,__FUNCTION__);
		addTerminal(); 
		StatementList();
		Expect(_T_Rbrace,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::ArrayAccess() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_ArrayAccess, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_identifier,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Lbrack,__FUNCTION__);
		addTerminal(); 
		Expr();
		Expect(_T_Rbrack,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::ScopeAccess() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_ScopeAccess, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_identifier,__FUNCTION__);
		addTerminal(); 
		Expect(_T_Dot,__FUNCTION__);
		addTerminal(); 
		Expect(_T_identifier,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::UnaryExpr() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_UnaryExpr, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (StartOf(3)) {
			PrimaryExpr();
		} else if (la->kind == _T_Bang) {
			UnaryOp();
			UnaryExpr();
		} else SynErr(41,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::Expr_nlr_() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Expr_nlr_, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		BinaryOp();
		Expr();
		if (StartOf(2)) {
			Expr_nlr_();
		}
		d_stack.pop(); 
}

void Parser::BinaryOp() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_BinaryOp, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		switch (la->kind) {
		case _T_Plus: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Minus: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Lt: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Leq: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Gt: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_Geq: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_2Eq: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_BangEq: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_2Amp: {
			Get();
			addTerminal(); 
			break;
		}
		case _T_2Bar: {
			Get();
			addTerminal(); 
			break;
		}
		default: SynErr(42,__FUNCTION__); break;
		}
		d_stack.pop(); 
}

void Parser::PrimaryExpr() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_PrimaryExpr, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (peek(1) == _T_identifier && peek(2) == _T_Lbrack ) {
			ArrayAccess();
		} else if (peek(1) == _T_identifier && peek(2) == _T_Dot ) {
			ScopeAccess();
		} else if (peek(1) == _T_identifier && peek(2) == _T_Lpar ) {
			Call();
		} else if (la->kind == _T_identifier) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_Minus || la->kind == _T_integer) {
			signed_();
		} else if (la->kind == _T_string) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_Lbrace) {
			Scope_();
		} else if (la->kind == _T_true) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_false) {
			Get();
			addTerminal(); 
		} else if (la->kind == _T_Lpar) {
			Get();
			addTerminal(); 
			Expr();
			Expect(_T_Rpar,__FUNCTION__);
			addTerminal(); 
		} else if (la->kind == _T_Lbrack) {
			List_();
		} else SynErr(43,__FUNCTION__);
		d_stack.pop(); 
}

void Parser::UnaryOp() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_UnaryOp, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_Bang,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::signed_() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_signed_, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		if (la->kind == _T_Minus) {
			Get();
			addTerminal(); 
		}
		Expect(_T_integer,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}

void Parser::Scope_() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_Scope_, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Block();
		d_stack.pop(); 
}

void Parser::List_() {
		Gn::SynTree* n = new Gn::SynTree( Gn::SynTree::R_List_, d_next ); d_stack.top()->d_children.append(n); d_stack.push(n); 
		Expect(_T_Lbrack,__FUNCTION__);
		addTerminal(); 
		while (StartOf(1)) {
			Expr();
			if (la->kind == _T_Comma) {
				Get();
				addTerminal(); 
			}
		}
		Expect(_T_Rbrack,__FUNCTION__);
		addTerminal(); 
		d_stack.pop(); 
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	d_cur = PARSER_NS::Token();
	d_next = PARSER_NS::Token();
	Get();
	File();
	Expect(0,__FUNCTION__);
}

Parser::Parser(PARSER_NS::Lexer *scanner, PARSER_NS::Errors* err) {
	maxT = 37;

	ParserInitCaller<Parser>::CallInit(this);
	la = &d_dummy;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = err;
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[4][39] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,T,x, x,x,T,x, x,x,x,T, x,x,x,x, x,x,x,x, T,x,T,x, x,x,x,T, x,T,x,T, T,T,x,x, x,x,x},
		{x,x,x,T, x,T,x,x, T,x,x,T, x,x,T,T, x,T,T,T, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,T,x, x,x,x,T, x,x,x,x, x,x,x,x, T,x,T,x, x,x,x,T, x,T,x,T, T,T,x,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
}

void Parser::SynErr(const QString& sourcePath, int line, int col, int n, PARSER_NS::Errors* err, const char* ctx, const QString& str ) {
	QString s;
	QString ctxStr;
	if( ctx )
		ctxStr = QString( " in %1" ).arg(ctx);
    if( n == 0 )
        s = QString("EOF expected%1").arg(ctxStr);
    else if( n < PARSER_NS::TT_Specials )
        s = QString("'%2' expected%1").arg(ctxStr).arg(PARSER_NS::tokenTypeString(n));
    else if( n <= PARSER_NS::TT_Max )
        s = QString("%2 expected%1").arg(ctxStr).arg(PARSER_NS::tokenTypeString(n));
    else
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"T_Literals_ expected"); break;
			case 2: s = coco_string_create(L"T_Bang expected"); break;
			case 3: s = coco_string_create(L"T_BangEq expected"); break;
			case 4: s = coco_string_create(L"T_Hash expected"); break;
			case 5: s = coco_string_create(L"T_2Amp expected"); break;
			case 6: s = coco_string_create(L"T_Lpar expected"); break;
			case 7: s = coco_string_create(L"T_Rpar expected"); break;
			case 8: s = coco_string_create(L"T_Plus expected"); break;
			case 9: s = coco_string_create(L"T_PlusEq expected"); break;
			case 10: s = coco_string_create(L"T_Comma expected"); break;
			case 11: s = coco_string_create(L"T_Minus expected"); break;
			case 12: s = coco_string_create(L"T_MinusEq expected"); break;
			case 13: s = coco_string_create(L"T_Dot expected"); break;
			case 14: s = coco_string_create(L"T_Lt expected"); break;
			case 15: s = coco_string_create(L"T_Leq expected"); break;
			case 16: s = coco_string_create(L"T_Eq expected"); break;
			case 17: s = coco_string_create(L"T_2Eq expected"); break;
			case 18: s = coco_string_create(L"T_Gt expected"); break;
			case 19: s = coco_string_create(L"T_Geq expected"); break;
			case 20: s = coco_string_create(L"T_Lbrack expected"); break;
			case 21: s = coco_string_create(L"T_Rbrack expected"); break;
			case 22: s = coco_string_create(L"T_Lbrace expected"); break;
			case 23: s = coco_string_create(L"T_2Bar expected"); break;
			case 24: s = coco_string_create(L"T_Rbrace expected"); break;
			case 25: s = coco_string_create(L"T_Keywords_ expected"); break;
			case 26: s = coco_string_create(L"T_else expected"); break;
			case 27: s = coco_string_create(L"T_false expected"); break;
			case 28: s = coco_string_create(L"T_if expected"); break;
			case 29: s = coco_string_create(L"T_true expected"); break;
			case 30: s = coco_string_create(L"T_Specials_ expected"); break;
			case 31: s = coco_string_create(L"T_identifier expected"); break;
			case 32: s = coco_string_create(L"T_integer expected"); break;
			case 33: s = coco_string_create(L"T_string expected"); break;
			case 34: s = coco_string_create(L"T_Comment expected"); break;
			case 35: s = coco_string_create(L"T_Eof expected"); break;
			case 36: s = coco_string_create(L"T_MaxToken_ expected"); break;
			case 37: s = coco_string_create(L"??? expected"); break;
			case 38: s = coco_string_create(L"invalid Statement"); break;
			case 39: s = coco_string_create(L"invalid Condition"); break;
			case 40: s = coco_string_create(L"invalid AssignOp"); break;
			case 41: s = coco_string_create(L"invalid UnaryExpr"); break;
			case 42: s = coco_string_create(L"invalid BinaryOp"); break;
			case 43: s = coco_string_create(L"invalid PrimaryExpr"); break;

		default:
		{
			s = QString( "generic error %1").arg(n);
		}
		break;
	}
    if( !str.isEmpty() )
        s = QString("%1 %2").arg(s).arg(str);
	if( err )
		err->error(PARSER_NS::Errors::Syntax, sourcePath, line, col, s);
	else
		qCritical() << "Error Parser" << line << col << s;
	//count++;
}

} // namespace
