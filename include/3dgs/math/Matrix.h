#pragma once

#include "pch.h"

namespace iiixrlab::math
{
	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	class Matrix final
	{
	public:
		static constexpr ElementType Dot(const Matrix& lhs, const Matrix& rhs) noexcept;
		static constexpr Matrix Transpose(const Matrix& matrix) noexcept;
		static constexpr Matrix Normalize(const Matrix& matrix) noexcept;

	public:
		static const Matrix IDENTITY;

	public:
		constexpr Matrix() noexcept;

		explicit constexpr Matrix(const ElementType* elements) noexcept;
		explicit constexpr Matrix(std::initializer_list<ElementType> elements) noexcept;

		IIIXRLAB_INLINE constexpr Matrix(const Matrix&) noexcept = default;
		IIIXRLAB_INLINE constexpr Matrix& operator=(const Matrix&) noexcept = default;

		constexpr Matrix& operator=(const ElementType* elements) noexcept;

		IIIXRLAB_INLINE constexpr ~Matrix() noexcept = default;

		IIIXRLAB_INLINE constexpr Matrix(Matrix&& other) noexcept = default;
		IIIXRLAB_INLINE constexpr Matrix& operator=(Matrix&&) noexcept = default;

		IIIXRLAB_INLINE constexpr Matrix& operator*=(const ElementType& scalar) noexcept;
		IIIXRLAB_INLINE constexpr Matrix& operator/=(const ElementType& scalar) noexcept;
		IIIXRLAB_INLINE constexpr Matrix& operator+=(const Matrix& other) noexcept;
		IIIXRLAB_INLINE constexpr Matrix& operator-=(const Matrix& other) noexcept;

		IIIXRLAB_INLINE constexpr bool operator==(const Matrix& other) const noexcept;
		IIIXRLAB_INLINE constexpr bool operator!=(const Matrix& other) const noexcept { return !(*this == other); }

		IIIXRLAB_INLINE constexpr ElementType* operator[](const uint8_t rowIndex) noexcept { return mElements[rowIndex]; }
		IIIXRLAB_INLINE constexpr const ElementType* operator[](const uint8_t rowIndex) const noexcept { return mElements[rowIndex]; }

		IIIXRLAB_INLINE constexpr ElementType& operator()(const uint8_t rowIndex, const uint8_t columnIndex) noexcept { return mElements[rowIndex][columnIndex]; }
		IIIXRLAB_INLINE constexpr const ElementType& operator()(const uint8_t rowIndex, const uint8_t columnIndex) const noexcept { return mElements[rowIndex][columnIndex]; }

		IIIXRLAB_INLINE constexpr uint8_t GetRowSize() const noexcept { return RowSize; }
		IIIXRLAB_INLINE constexpr uint8_t GetColumnSize() const noexcept { return ColumnSize; }
		IIIXRLAB_INLINE constexpr uint8_t GetTotalSize() const noexcept { return RowSize * ColumnSize; }

		IIIXRLAB_INLINE constexpr ElementType* GetElements() noexcept { return &mElements[0][0]; }
		IIIXRLAB_INLINE constexpr const ElementType* GetElements() const noexcept { return &mElements[0][0]; }

		IIIXRLAB_INLINE constexpr ElementType Dot(const Matrix& matrix) const noexcept { return Dot(*this, matrix); }
		
        IIIXRLAB_INLINE constexpr ElementType GetSizeSquared() const noexcept { return Dot(*this); }
        IIIXRLAB_INLINE constexpr ElementType GetSize() const noexcept { return std::sqrt(GetSizeSquared()); }
        IIIXRLAB_INLINE constexpr bool IsSizeZero() const noexcept { return GetSizeSquared() == 0.0f; }

		IIIXRLAB_INLINE constexpr ElementType& GetX() noexcept { return mElements[0][0]; }
		IIIXRLAB_INLINE constexpr const ElementType& GetX() const noexcept { return mElements[0][0]; }
		IIIXRLAB_INLINE constexpr ElementType& GetY() noexcept { return mElements[0][1]; }
		IIIXRLAB_INLINE constexpr const ElementType& GetY() const noexcept { return mElements[0][1]; }
		IIIXRLAB_INLINE constexpr ElementType& GetZ() noexcept { return mElements[0][2]; }
		IIIXRLAB_INLINE constexpr const ElementType& GetZ() const noexcept { return mElements[0][2]; }
		IIIXRLAB_INLINE constexpr ElementType& GetW() noexcept { return mElements[0][3]; }
		IIIXRLAB_INLINE constexpr const ElementType& GetW() const noexcept { return mElements[0][3]; }
	
	protected:
		ElementType mElements[RowSize][ColumnSize];
	};

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	constexpr Matrix<RowSize, ColumnSize, ElementType> operator*(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const ElementType& scalar) noexcept;

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	constexpr Matrix<RowSize, ColumnSize, ElementType> operator*(const ElementType& lhs, const Matrix<RowSize, ColumnSize, ElementType>& scalar) noexcept;

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	constexpr Matrix<RowSize, ColumnSize, ElementType> operator+(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept;

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	constexpr Matrix<RowSize, ColumnSize, ElementType> operator-(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept;

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	constexpr Matrix<RowSize, ColumnSize, ElementType> operator/(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const ElementType& scalar) noexcept;

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	constexpr bool operator==(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept;

	template<uint8_t RowSize, uint8_t ColumnSize, Arithmetic ElementType>
	IIIXRLAB_INLINE constexpr bool operator!=(const Matrix<RowSize, ColumnSize, ElementType>& lhs, const Matrix<RowSize, ColumnSize, ElementType>& rhs) noexcept { return !(lhs == rhs); }

	template<Arithmetic ElementType>
	using Matrix2x2 = Matrix<2, 2, ElementType>;

	using Matrix2x2f = Matrix2x2<float>;
	using Matrix2x2d = Matrix2x2<double>;
	using Matrix2x2i = Matrix2x2<int32_t>;
	using Matrix2x2u = Matrix2x2<uint32_t>;

	template<Arithmetic ElementType>
	using Matrix3x3 = Matrix<3, 3, ElementType>;
	
	using Matrix3x3f = Matrix3x3<float>;
	using Matrix3x3d = Matrix3x3<double>;
	using Matrix3x3i = Matrix3x3<int32_t>;
	using Matrix3x3u = Matrix3x3<uint32_t>;

	template<Arithmetic ElementType>
	using Matrix4x4 = Matrix<4, 4, ElementType>;

	using Matrix4x4f = Matrix4x4<float>;
	using Matrix4x4d = Matrix4x4<double>;
	using Matrix4x4i = Matrix4x4<int32_t>;
	using Matrix4x4u = Matrix4x4<uint32_t>;
}   // namespace iiixrlab::math