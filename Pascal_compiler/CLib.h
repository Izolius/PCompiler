#pragma once

struct CTextPosition
{
	size_t m_line; //����� ������
	size_t m_pos; //����� �������
	CTextPosition(size_t line, size_t pos) :m_line(line), m_pos(pos) {}
	CTextPosition() :CTextPosition(0, 0) {}
};
