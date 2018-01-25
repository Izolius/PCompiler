#pragma once

enum ETokenCode {
	star, // *
	//sharp, // #
	slash, // /
	equal, // =
	comma, // ,
	semicolon, // ;
	colon, // :
	point, // .
	arrow, // ^
	leftpar, // (
	rightpar, // )
	lbracket, // [
	rbracket, // ]
	flpar, // {
	frpar, // }
	later, // <
	greater, // >
	laterequal, // <>
	graterequal, // <=
	latergrater, // >=
	plus, // +
	minus, // -
	lcomment, // (*
	rcomment, // *)
	assign, // :=
	twopoints, // ..
	ident, 
	floatc, 
	intc, 
	charc,
	//ключевые слова
	ifsy, 
	dosy, 
	ofsy, 
	orsy, 
	insy, 
	tosy, 
	endsy, 
	varsy, 
	divsy, 
	andsy, 
	notsy, 
	forsy, 
	modsy, 
	nilsy, 
	setsy,
	thensy, 
	elsesy, 
	casesy, 
	filesy, 
	fotosy, 
	typesy, 
	withsy,
	beginsy, 
	whilesy, 
	arraysy, 
	constsy, 
	labelsy, 
	untilsy, 
	downtosy, 
	packedsy, 
	recordsy, 
	repeatsy, 
	propgramsy, 
	funcsy, 
	procsy,
	ERROR
};