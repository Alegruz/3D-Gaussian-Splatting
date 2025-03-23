#pragma once

#include "3dgs/math/Matrix.h"


namespace iiixrlab::math
{
	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr ElementType Matrix<RowSize, ColumnSize, ElementType>::Dot(const Matrix& lhs, const Matrix& rhs) noexcept
	{
		ElementType result = 0;

		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			result = lhs.mElements[0][0] * rhs.mElements[0][0];
		}
		else if constexpr(RowSize == 1 && ColumnSize > 1)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				result += lhs.mElements[0][columnIndex] * rhs.mElements[0][columnIndex];
			}
		}
		else if constexpr(RowSize > 1 && ColumnSize == 1)
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				result += lhs.mElements[rowIndex][0] * rhs.mElements[rowIndex][0];
			}
		}
		else
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
				{
					result += lhs.mElements[rowIndex][columnIndex] * rhs.mElements[rowIndex][columnIndex];
				}
			}
		}

		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType> Matrix<RowSize, ColumnSize, ElementType>::Transpose(const Matrix& matrix) noexcept
	{
		Matrix<ColumnSize, RowSize, ElementType> result;

		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			result.mElements[0][0] = matrix.mElements[0][0];
			return result;
		}

		for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				result.mElements[columnIndex][rowIndex] = matrix.mElements[rowIndex][columnIndex];
			}
		}

		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType> Matrix<RowSize, ColumnSize, ElementType>::Normalize(const Matrix& matrix) noexcept
	{
		Matrix<RowSize, ColumnSize, ElementType> result;

		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			result.mElements[0][0] = matrix.mElements[0][0] / matrix.mElements[0][0];
			return result;
		}

		const ElementType length = std::sqrt(matrix.Dot(matrix));

		if(length == 0)
		{
			return result;
		}

		for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				result.mElements[rowIndex][columnIndex] = matrix.mElements[rowIndex][columnIndex] / length;
			}
		}

		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	const Matrix<RowSize, ColumnSize, ElementType> Matrix<RowSize, ColumnSize, ElementType>::IDENTITY;

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>::Matrix() noexcept
		: mElements()
	{
		std::memset(mElements, 0, sizeof(mElements));

		if constexpr(RowSize > 1 && ColumnSize > 1)
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
				{
					if(rowIndex == columnIndex)
					{
						mElements[rowIndex][columnIndex] = 1;
					}
				}
			}
		}
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>::Matrix(const ElementType* elements) noexcept
		: mElements()
	{
		std::memcpy(mElements, elements, sizeof(mElements));
	}
	
	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>::Matrix(std::initializer_list<ElementType> elements) noexcept
		: mElements()
	{
		std::memcpy(mElements, elements.begin(), sizeof(mElements));
	}
	
	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>& Matrix<RowSize, ColumnSize, ElementType>::operator=(const ElementType* elements) noexcept
	{
		std::memcpy(mElements, elements, sizeof(mElements));
		return *this;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>& Matrix<RowSize, ColumnSize, ElementType>::operator*=(const ElementType& scalar) noexcept
	{
		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			mElements[0][0] *= scalar;
			return *this;
		}
		else if constexpr(RowSize == 1 && ColumnSize > 1)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				mElements[0][columnIndex] *= scalar;
			}
			return *this;
		}
		else if constexpr(RowSize > 1 && ColumnSize == 1)
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				mElements[rowIndex][0] *= scalar;
			}
			return *this;
		}
		else
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
				{
					mElements[rowIndex][columnIndex] *= scalar;
				}
			}
			return *this;
		}
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>& Matrix<RowSize, ColumnSize, ElementType>::operator/=(const ElementType& scalar) noexcept
	{
		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			mElements[0][0] /= scalar;
			return *this;
		}
		else if constexpr(RowSize == 1 && ColumnSize > 1)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				mElements[0][columnIndex] /= scalar;
			}
			return *this;
		}
		else if constexpr(RowSize > 1 && ColumnSize == 1)
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				mElements[rowIndex][0] /= scalar;
			}
			return *this;
		}
		else
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
				{
					mElements[rowIndex][columnIndex] /= scalar;
				}
			}
			return *this;
		}
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>& Matrix<RowSize, ColumnSize, ElementType>::operator+=(const Matrix& other) noexcept
	{
		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			mElements[0][0] += other.mElements[0][0];
			return *this;
		}
		else if constexpr(RowSize == 1 && ColumnSize > 1)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				mElements[0][columnIndex] += other.mElements[0][columnIndex];
			}
			return *this;
		}
		else if constexpr(RowSize > 1 && ColumnSize == 1)
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				mElements[rowIndex][0] += other.mElements[rowIndex][0];
			}
			return *this;
		}
		else
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
				{
					mElements[rowIndex][columnIndex] += other.mElements[rowIndex][columnIndex];
				}
			}
			return *this;
		}
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType>& Matrix<RowSize, ColumnSize, ElementType>::operator-=(const Matrix& other) noexcept
	{
		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			mElements[0][0] -= other.mElements[0][0];
			return *this;
		}
		else if constexpr(RowSize == 1 && ColumnSize > 1)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				mElements[0][columnIndex] -= other.mElements[0][columnIndex];
			}
			return *this;
		}
		else if constexpr(RowSize > 1 && ColumnSize == 1)
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				mElements[rowIndex][0] -= other.mElements[rowIndex][0];
			}
			return *this;
		}
		else
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
				{
					mElements[rowIndex][columnIndex] -= other.mElements[rowIndex][columnIndex];
				}
			}
			return *this;
		}
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr bool Matrix<RowSize, ColumnSize, ElementType>::operator==(const Matrix& other) const noexcept
	{
		if constexpr(RowSize == 1 && ColumnSize == 1)
		{
			return mElements[0][0] == other.mElements[0][0];
		}
		else if constexpr(RowSize == 1 && ColumnSize > 1)
		{
			for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
			{
				if(mElements[0][columnIndex] != other.mElements[0][columnIndex])
				{
					return false;
				}
			}
			return true;
		}
		else if constexpr(RowSize > 1 && ColumnSize == 1)
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				if(mElements[rowIndex][0] != other.mElements[rowIndex][0])
				{
					return false;
				}
			}
			return true;
		}
		else
		{
			for(uint8_t rowIndex = 0; rowIndex < RowSize; ++rowIndex)
			{
				for(uint8_t columnIndex = 0; columnIndex < ColumnSize; ++columnIndex)
				{
					if(mElements[rowIndex][columnIndex] != other.mElements[rowIndex][columnIndex])
					{
						return false;
					}
				}
			}
			return true;
		}
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType> operator*(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const ElementType& scalar) noexcept
	{
		Matrix<RowSize, ColumnSize, ElementType> result(lhs);
		result *= scalar;
		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType> operator*(const ElementType& scalar, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept
	{
		Matrix<RowSize, ColumnSize, ElementType> result(rhs);
		result *= scalar;
		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType> operator+(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept
	{
		Matrix<RowSize, ColumnSize, ElementType> result(lhs);
		result += rhs;
		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType> operator-(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept
	{
		Matrix<RowSize, ColumnSize, ElementType> result(lhs);
		result -= rhs;
		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr Matrix<RowSize, ColumnSize, ElementType> operator/(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const ElementType& scalar) noexcept
	{
		Matrix<RowSize, ColumnSize, ElementType> result(lhs);
		result /= scalar;
		return result;
	}

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr bool operator==(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept
	{
		return lhs.operator==(rhs);
	}
}   // namespace iiixrlab::math