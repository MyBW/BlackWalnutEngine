#ifndef AXISALIGNEDBOX_H_
#define AXISALIGNEDBOX_H_

#define NOMINMAX
#include "BWPrimitive.h"
#include "Math.h"
#include <limits>


class  AxisAlignedBox
{
public:
	enum Extent
	{
		EXTENT_NULL,
		EXTENT_FINITE,
		EXTENT_INFINITE
	};
protected:

	BWVector3 mMinimum;
	BWVector3 mMaximum;
	Extent mExtent;
	mutable BWVector3* mCorners;

public:
	/*
	1-----2
	/|    /|
	/ |   / |
	5-----4  |
	|  0--|--3
	| /   | /
	|/    |/
	6-----7
	*/
	typedef enum {
		FAR_LEFT_BOTTOM = 0,
		FAR_LEFT_TOP = 1,
		FAR_RIGHT_TOP = 2,
		FAR_RIGHT_BOTTOM = 3,
		NEAR_RIGHT_BOTTOM = 7,
		NEAR_LEFT_BOTTOM = 6,
		NEAR_LEFT_TOP = 5,
		NEAR_RIGHT_TOP = 4
	} CornerEnum;
	inline AxisAlignedBox() : mMinimum(BWVector3::ZERO), mMaximum(BWVector3::UNIT_SCALE), mCorners(0)
	{
		// Default to a null box 
		setMinimum(-0.5, -0.5, -0.5);
		setMaximum(0.5, 0.5, 0.5);
		mExtent = EXTENT_NULL;
	}
	inline AxisAlignedBox(Extent e) : mMinimum(BWVector3::ZERO), mMaximum(BWVector3::UNIT_SCALE), mCorners(0)
	{
		setMinimum(-0.5, -0.5, -0.5);
		setMaximum(0.5, 0.5, 0.5);
		mExtent = e;
	}

	inline AxisAlignedBox(const AxisAlignedBox & rkBox) : mMinimum(BWVector3::ZERO), mMaximum(BWVector3::UNIT_SCALE), mCorners(0)

	{
		if (rkBox.isNull())
			setNull();
		else if (rkBox.isInfinite())
			setInfinite();
		else
			setExtents(rkBox.mMinimum, rkBox.mMaximum);
	}

	inline AxisAlignedBox(const BWVector3& min, const BWVector3& max) : mMinimum(BWVector3::ZERO), mMaximum(BWVector3::UNIT_SCALE), mCorners(0)
	{
		setExtents(min, max);
	}

	inline AxisAlignedBox(
		float mx, float my, float mz,
		float Mx, float My, float Mz) : mMinimum(BWVector3::ZERO), mMaximum(BWVector3::UNIT_SCALE), mCorners(0)
	{
		setExtents(mx, my, mz, Mx, My, Mz);
	}

	AxisAlignedBox& operator=(const AxisAlignedBox& rhs)
	{
		// Specifically override to avoid copying mCorners
		if (rhs.isNull())
			setNull();
		else if (rhs.isInfinite())
			setInfinite();
		else
			setExtents(rhs.mMinimum, rhs.mMaximum);

		return *this;
	}

	~AxisAlignedBox()
	{
		if (mCorners)
			delete[] mCorners;
	}


	/** Gets the minimum corner of the box.
	*/
	inline const BWVector3& getMinimum(void) const
	{
		return mMinimum;
	}

	/** Gets a modifiable version of the minimum
	corner of the box.
	*/
	inline BWVector3& getMinimum(void)
	{
		return mMinimum;
	}

	/** Gets the maximum corner of the box.
	*/
	inline const BWVector3& getMaximum(void) const
	{
		return mMaximum;
	}

	/** Gets a modifiable version of the maximum
	corner of the box.
	*/
	inline BWVector3& getMaximum(void)
	{
		return mMaximum;
	}


	/** Sets the minimum corner of the box.
	*/
	inline void setMinimum(const BWVector3& vec)
	{
		mExtent = EXTENT_FINITE;
		mMinimum = vec;
	}

	inline void setMinimum(float x, float y, float z)
	{
		mExtent = EXTENT_FINITE;
		mMinimum.x = x;
		mMinimum.y = y;
		mMinimum.z = z;
	}

	/** Changes one of the components of the minimum corner of the box
	used to resize only one dimension of the box
	*/
	inline void setMinimumX(float x)
	{
		mMinimum.x = x;
	}

	inline void setMinimumY(float y)
	{
		mMinimum.y = y;
	}

	inline void setMinimumZ(float z)
	{
		mMinimum.z = z;
	}

	/** Sets the maximum corner of the box.
	*/
	inline void setMaximum(const BWVector3& vec)
	{
		mExtent = EXTENT_FINITE;
		mMaximum = vec;
	}

	inline void setMaximum(float x, float y, float z)
	{
		mExtent = EXTENT_FINITE;
		mMaximum.x = x;
		mMaximum.y = y;
		mMaximum.z = z;
	}

	/** Changes one of the components of the maximum corner of the box
	used to resize only one dimension of the box
	*/
	inline void setMaximumX(float x)
	{
		mMaximum.x = x;
	}

	inline void setMaximumY(float y)
	{
		mMaximum.y = y;
	}

	inline void setMaximumZ(float z)
	{
		mMaximum.z = z;
	}

	/** Sets both minimum and maximum extents at once.
	*/
	inline void setExtents(const BWVector3& min, const BWVector3& max)
	{
		assert((min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
			"The minimum corner of the box must be less than or equal to maximum corner");

		mExtent = EXTENT_FINITE;
		mMinimum = min;
		mMaximum = max;
	}

	inline void setExtents(
		float mx, float my, float mz,
		float Mx, float My, float Mz)
	{
		assert((mx <= Mx && my <= My && mz <= Mz) &&
			"The minimum corner of the box must be less than or equal to maximum corner");

		mExtent = EXTENT_FINITE;

		mMinimum.x = mx;
		mMinimum.y = my;
		mMinimum.z = mz;

		mMaximum.x = Mx;
		mMaximum.y = My;
		mMaximum.z = Mz;

	}

	/** Returns a pointer to an array of 8 corner points, useful for
	collision vs. non-aligned objects.
	@remarks
	If the order of these corners is important, they are as
	follows: The 4 points of the minimum Z face (note that
	because Ogre uses right-handed coordinates, the minimum Z is
	at the 'back' of the box) starting with the minimum point of
	all, then anticlockwise around this face (if you are looking
	onto the face from outside the box). Then the 4 points of the
	maximum Z face, starting with maximum point of all, then
	anticlockwise around this face (looking onto the face from
	outside the box). Like this:
	<pre>
	1-----2
	/|    /|
	/ |   / |
	5-----4  |
	|  0--|--3
	| /   | /
	|/    |/
	6-----7
	</pre>
	@remarks as this implementation uses a static member, make sure to use your own copy !
	*/
	inline const BWVector3* getAllCorners(void) const
	{
		assert((mExtent == EXTENT_FINITE) && "Can't get corners of a null or infinite AAB");

		// The order of these items is, using right-handed co-ordinates:
		// Minimum Z face, starting with Min(all), then anticlockwise
		//   around face (looking onto the face)
		// Maximum Z face, starting with Max(all), then anticlockwise
		//   around face (looking onto the face)
		// Only for optimization/compatibility.
		if (!mCorners)
			mCorners = new BWVector3[8];

		mCorners[0] = mMinimum;
		mCorners[1].x = mMinimum.x; mCorners[1].y = mMaximum.y; mCorners[1].z = mMinimum.z;
		mCorners[2].x = mMaximum.x; mCorners[2].y = mMaximum.y; mCorners[2].z = mMinimum.z;
		mCorners[3].x = mMaximum.x; mCorners[3].y = mMinimum.y; mCorners[3].z = mMinimum.z;

		mCorners[4] = mMaximum;
		mCorners[5].x = mMinimum.x; mCorners[5].y = mMaximum.y; mCorners[5].z = mMaximum.z;
		mCorners[6].x = mMinimum.x; mCorners[6].y = mMinimum.y; mCorners[6].z = mMaximum.z;
		mCorners[7].x = mMaximum.x; mCorners[7].y = mMinimum.y; mCorners[7].z = mMaximum.z;

		return mCorners;
	}

	/** gets the position of one of the corners
	*/
	BWVector3 getCorner(CornerEnum cornerToGet) const
	{
		switch (cornerToGet)
		{
		case FAR_LEFT_BOTTOM:
			return mMinimum;
		case FAR_LEFT_TOP:
			return BWVector3(mMinimum.x, mMaximum.y, mMinimum.z);
		case FAR_RIGHT_TOP:
			return BWVector3(mMaximum.x, mMaximum.y, mMinimum.z);
		case FAR_RIGHT_BOTTOM:
			return BWVector3(mMaximum.x, mMinimum.y, mMinimum.z);
		case NEAR_RIGHT_BOTTOM:
			return BWVector3(mMaximum.x, mMinimum.y, mMaximum.z);
		case NEAR_LEFT_BOTTOM:
			return BWVector3(mMinimum.x, mMinimum.y, mMaximum.z);
		case NEAR_LEFT_TOP:
			return BWVector3(mMinimum.x, mMaximum.y, mMaximum.z);
		case NEAR_RIGHT_TOP:
			return mMaximum;
		default:
			return BWVector3();
		}
	}

	friend std::ostream& operator<<(std::ostream& o, const AxisAlignedBox aab)
	{
		//switch (aab.mExtent)
		//{
		//case EXTENT_NULL:
		//	o << "AxisAlignedBox(null)";
		//	return o;

		//case EXTENT_FINITE:
		//	o << "AxisAlignedBox(min=" << aab.mMinimum << ", max=" << aab.mMaximum << ")";
		//	return o;

		//case EXTENT_INFINITE:
		//	o << "AxisAlignedBox(infinite)";
		//	return o;

		//default: // shut up compiler
		//	assert(false && "Never reached");
		//	return o;
		//}
	}

	/** Merges the passed in box into the current box. The result is the
	box which encompasses both.
	*/
	void merge(const AxisAlignedBox& rhs)
	{
		// Do nothing if rhs null, or this is infinite
		if ((rhs.mExtent == EXTENT_NULL) || (mExtent == EXTENT_INFINITE))
		{
			return;
		}
		// Otherwise if rhs is infinite, make this infinite, too
		else if (rhs.mExtent == EXTENT_INFINITE)
		{
			mExtent = EXTENT_INFINITE;
		}
		// Otherwise if current null, just take rhs
		else if (mExtent == EXTENT_NULL)
		{
			setExtents(rhs.mMinimum, rhs.mMaximum);
		}
		// Otherwise merge
		else
		{
			BWVector3 min = mMinimum;
			BWVector3 max = mMaximum;
			max.makeCeil(rhs.mMaximum);
			min.makeFloor(rhs.mMinimum);

			setExtents(min, max);
		}

	}

	/** Extends the box to encompass the specified point (if needed).
	*/
	inline void merge(const BWVector3& point)
	{
		switch (mExtent)
		{
		case EXTENT_NULL: // if null, use this point
			setExtents(point, point);
			return;

		case EXTENT_FINITE:
			mMaximum.makeCeil(point);
			mMinimum.makeFloor(point);
			return;

		case EXTENT_INFINITE: // if infinite, makes no difference
			return;
		}

		assert(false && "Never reached");
	}

	/** Transforms the box according to the matrix supplied.
	@remarks
	By calling this method you get the axis-aligned box which
	surrounds the transformed version of this box. Therefore each
	corner of the box is transformed by the matrix, then the
	extents are mapped back onto the axes to produce another
	AABB. Useful when you have a local AABB for an object which
	is then transformed.
	*/
	inline void transform(const BWMatrix4& matrix)
	{
		// Do nothing if current null or infinite
		if (mExtent != EXTENT_FINITE)
			return;

		BWVector3 oldMin, oldMax, currentCorner;

		// Getting the old values so that we can use the existing merge method.
		oldMin = mMinimum;
		oldMax = mMaximum;

		// reset
		setNull();

		// We sequentially compute the corners in the following order :
		// 0, 6, 5, 1, 2, 4 ,7 , 3
		// This sequence allows us to only change one member at a time to get at all corners.

		// For each one, we transform it using the matrix
		// Which gives the resulting point and merge the resulting point.

		// First corner 
		// min min min
		currentCorner = oldMin;
		merge(matrix * currentCorner);

		// min,min,max
		currentCorner.z = oldMax.z;
		merge(matrix * currentCorner);

		// min max max
		currentCorner.y = oldMax.y;
		merge(matrix * currentCorner);

		// min max min
		currentCorner.z = oldMin.z;
		merge(matrix * currentCorner);

		// max max min
		currentCorner.x = oldMax.x;
		merge(matrix * currentCorner);

		// max max max
		currentCorner.z = oldMax.z;
		merge(matrix * currentCorner);

		// max min max
		currentCorner.y = oldMin.y;
		merge(matrix * currentCorner);

		// max min min
		currentCorner.z = oldMin.z;
		merge(matrix * currentCorner);
	}

	/** Transforms the box according to the affine matrix supplied.
	@remarks
	By calling this method you get the axis-aligned box which
	surrounds the transformed version of this box. Therefore each
	corner of the box is transformed by the matrix, then the
	extents are mapped back onto the axes to produce another
	AABB. Useful when you have a local AABB for an object which
	is then transformed.
	@note
	The matrix must be an affine matrix. @see Matrix4::isAffine.
	*/
	void transformAffine(const BWMatrix4& m)
	{
		assert(m.isAffine());

		// Do nothing if current null or infinite
		if (mExtent != EXTENT_FINITE)
			return;

		BWVector3 centre = getCenter();
		BWVector3 halfSize = getHalfSize();
	
		BWVector3 newCentre = m.transformAffine(centre);
		BWVector3 newHalfSize(
			Math::Abs(m[0][0]) * halfSize.x + Math::Abs(m[0][1]) * halfSize.y + Math::Abs(m[0][2]) * halfSize.z,
			Math::Abs(m[1][0]) * halfSize.x + Math::Abs(m[1][1]) * halfSize.y + Math::Abs(m[1][2]) * halfSize.z,
			Math::Abs(m[2][0]) * halfSize.x + Math::Abs(m[2][1]) * halfSize.y + Math::Abs(m[2][2]) * halfSize.z);

		setExtents(newCentre - newHalfSize, newCentre + newHalfSize);
	}

	/** Sets the box to a 'null' value i.e. not a box.
	*/
	inline void setNull()
	{
		mExtent = EXTENT_NULL;
	}

	/** Returns true if the box is null i.e. empty.
	*/
	inline bool isNull(void) const
	{
		return (mExtent == EXTENT_NULL);
	}

	/** Returns true if the box is finite.
	*/
	bool isFinite(void) const
	{
		return (mExtent == EXTENT_FINITE);
	}

	/** Sets the box to 'infinite'
	*/
	inline void setInfinite()
	{
		mExtent = EXTENT_INFINITE;
	}

	/** Returns true if the box is infinite.
	*/
	bool isInfinite(void) const
	{
		return (mExtent == EXTENT_INFINITE);
	}

	/** Returns whether or not this box intersects another. */
	inline bool intersects(const AxisAlignedBox& b2) const
	{
		// Early-fail for nulls
		if (this->isNull() || b2.isNull())
			return false;

		// Early-success for infinites
		if (this->isInfinite() || b2.isInfinite())
			return true;

		// Use up to 6 separating planes
		if (mMaximum.x < b2.mMinimum.x)
			return false;
		if (mMaximum.y < b2.mMinimum.y)
			return false;
		if (mMaximum.z < b2.mMinimum.z)
			return false;

		if (mMinimum.x > b2.mMaximum.x)
			return false;
		if (mMinimum.y > b2.mMaximum.y)
			return false;
		if (mMinimum.z > b2.mMaximum.z)
			return false;

		// otherwise, must be intersecting
		return true;

	}

	/// Calculate the area of intersection of this box and another
	inline AxisAlignedBox intersection(const AxisAlignedBox& b2) const
	{
		if (this->isNull() || b2.isNull())
		{
			return AxisAlignedBox();
		}
		else if (this->isInfinite())
		{
			return b2;
		}
		else if (b2.isInfinite())
		{
			return *this;
		}

		BWVector3 intMin = mMinimum;
		BWVector3 intMax = mMaximum;

		intMin.makeCeil(b2.getMinimum());
		intMax.makeFloor(b2.getMaximum());

		// Check intersection isn't null
		if (intMin.x < intMax.x &&
			intMin.y < intMax.y &&
			intMin.z < intMax.z)
		{
			return AxisAlignedBox(intMin, intMax);
		}

		return AxisAlignedBox();
	}

	/// Calculate the volume of this box
	float volume(void) const
	{
		switch (mExtent)
		{
		case EXTENT_NULL:
			return 0.0f;

		case EXTENT_FINITE:
		{
			BWVector3 diff = mMaximum - mMinimum;
			return diff.x * diff.y * diff.z;
		}

		case EXTENT_INFINITE:
			return POS_INFINITY;

		default: // shut up compiler
			assert(false && "Never reached");
			return 0.0f;
		}
	}

	/** Scales the AABB by the vector given. */
	inline void scale(const BWVector3& s)
	{
		// Do nothing if current null or infinite
		if (mExtent != EXTENT_FINITE)
			return;

		// NB assumes centered on origin
		BWVector3 min = mMinimum * s;
		BWVector3 max = mMaximum * s;
		setExtents(min, max);
	}

	/** Tests whether this box intersects a sphere. */

	//bool intersects(const Sphere& s) const
	//{
	//	return Math::intersects(s, *this);
	//}
	///** Tests whether this box intersects a plane. */
	//bool intersects(const Plane& p) const
	//{
	//	return Math::intersects(p, *this);
	//}

	/** Tests whether the vector point is within this box. */
	bool intersects(const BWVector3& v) const
	{
		switch (mExtent)
		{
		case EXTENT_NULL:
			return false;

		case EXTENT_FINITE:
			return(v.x >= mMinimum.x  &&  v.x <= mMaximum.x  &&
				v.y >= mMinimum.y  &&  v.y <= mMaximum.y  &&
				v.z >= mMinimum.z  &&  v.z <= mMaximum.z);

		case EXTENT_INFINITE:
			return true;

		default: // shut up compiler
			assert(false && "Never reached");
			return false;
		}
	}
	/// Gets the centre of the box
	BWVector3 getCenter(void) const
	{
		assert((mExtent == EXTENT_FINITE) && "Can't get center of a null or infinite AAB");

		return BWVector3(
			(mMaximum.x + mMinimum.x) * 0.5f,
			(mMaximum.y + mMinimum.y) * 0.5f,
			(mMaximum.z + mMinimum.z) * 0.5f);
	}
	/// Gets the size of the box
	BWVector3 getSize(void) const
	{
		switch (mExtent)
		{
		case EXTENT_NULL:
			return BWVector3::ZERO;

		case EXTENT_FINITE:
			return mMaximum - mMinimum;

		case EXTENT_INFINITE:
			return BWVector3(
				POS_INFINITY,
				POS_INFINITY,
				POS_INFINITY);

		default: // shut up compiler
			assert(false && "Never reached");
			return BWVector3::ZERO;
		}
	}
	/// Gets the half-size of the box
	BWVector3 getHalfSize(void) const
	{
		switch (mExtent)
		{
		case EXTENT_NULL:
			return BWVector3::ZERO;

		case EXTENT_FINITE:
			return (mMaximum - mMinimum) * 0.5;

		case EXTENT_INFINITE:
			return BWVector3(
				POS_INFINITY,
				POS_INFINITY,
				POS_INFINITY);

		default: // shut up compiler
			assert(false && "Never reached");
			return BWVector3::ZERO;
		}
	}

	/** Tests whether the given point contained by this box.
	*/
	bool contains(const BWVector3& v) const
	{
		if (isNull())
			return false;
		if (isInfinite())
			return true;

		return mMinimum.x <= v.x && v.x <= mMaximum.x &&
			mMinimum.y <= v.y && v.y <= mMaximum.y &&
			mMinimum.z <= v.z && v.z <= mMaximum.z;
	}

	/** Returns the minimum distance between a given point and any part of the box. */
	float distance(const BWVector3& v) const
	{

		if (this->contains(v))
			return 0;
		else
		{
			float maxDist = (std::numeric_limits<float>::min)();

			if (v.x < mMinimum.x)
				maxDist = std::max<float>(maxDist, mMinimum.x - v.x);
			if (v.y < mMinimum.y)
				maxDist = std::max<float>(maxDist, mMinimum.y - v.y);
			if (v.z < mMinimum.z)
				maxDist = std::max<float>(maxDist, mMinimum.z - v.z);

			if (v.x > mMaximum.x)
				maxDist = std::max<float>(maxDist, v.x - mMaximum.x);
			if (v.y > mMaximum.y)
				maxDist = std::max<float>(maxDist, v.y - mMaximum.y);
			if (v.z > mMaximum.z)
				maxDist = std::max<float>(maxDist, v.z - mMaximum.z);

			return maxDist;
		}
	}

	/** Tests whether another box contained by this box.
	*/
	bool contains(const AxisAlignedBox& other) const
	{
		if (other.isNull() || this->isInfinite())
			return true;

		if (this->isNull() || other.isInfinite())
			return false;

		return this->mMinimum.x <= other.mMinimum.x &&
			this->mMinimum.y <= other.mMinimum.y &&
			this->mMinimum.z <= other.mMinimum.z &&
			other.mMaximum.x <= this->mMaximum.x &&
			other.mMaximum.y <= this->mMaximum.y &&
			other.mMaximum.z <= this->mMaximum.z;
	}

	/** Tests 2 boxes for equality.
	*/
	bool operator== (const AxisAlignedBox& rhs) const
	{
		if (this->mExtent != rhs.mExtent)
			return false;

		if (!this->isFinite())
			return true;

		return this->mMinimum == rhs.mMinimum &&
			this->mMaximum == rhs.mMaximum;
	}

	/** Tests 2 boxes for inequality.
	*/
	bool operator!= (const AxisAlignedBox& rhs) const
	{
		return !(*this == rhs);
	}

	// special values
	static const AxisAlignedBox BOX_NULL;
	static const AxisAlignedBox BOX_INFINITE;
};

#endif