#pragma once

#include <math.h>
#include "TVector.h"

namespace SAM 
{
	template <class TYPE, unsigned int ROWS, unsigned int COLUMNS>
	class TMatrix {

	private:

		TVector<TYPE, COLUMNS>										m_element[ROWS];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! Class destructor
																	~TMatrix();

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{
																		// can only default to identity matrix, if we are a square matrix
																		if (COLUMNS == ROWS)
																		{
																			unsigned int columnIndex = 0;
																			for (int rowIndex = 0; rowIndex < ROWS; ++rowIndex)
																				m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																		}
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < ROWS; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < COLUMNS; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}
	};

	//! Matrix 2x2
	template <class TYPE>
	class TMatrix<TYPE, 2, 2> {

	private:

		TVector<TYPE, 2>											m_element[2];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! overload the subscript operator
		TVector<TYPE, 2>											&operator[] (const unsigned int index)
																	{
																		return m_element[index];
																	}

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{																
																		unsigned int columnIndex = 0;
																		for (int rowIndex = 0; rowIndex < 2; ++rowIndex)
																			m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < 2; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < 2; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}
	};

	//! Matrix 3x3
	template <class TYPE>
	class TMatrix<TYPE, 3, 3> {

	private:

		TVector<TYPE, 3>											m_element[3];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! overload the subscript operator
		TVector<TYPE, 3>											&operator[] (const unsigned int index)
																	{
																		return m_element[index];
																	}

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{																
																		unsigned int columnIndex = 0;
																		for (int rowIndex = 0; rowIndex < 3; ++rowIndex)
																			m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < 3; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < 3; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}
	};

	//! Matrix 4x4
	template <class TYPE>
	class TMatrix<TYPE, 4, 4> {

	private:

		TVector<TYPE, 4>											m_element[4];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! overload the subscript operator
		TVector<TYPE, 4>											&operator[] (const unsigned int index)
																	{
																		return m_element[index];
																	}

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{																
																		unsigned int columnIndex = 0;
																		for (int rowIndex = 0; rowIndex < 4; ++rowIndex)
																			m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < 4; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < 4; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}
	};

}