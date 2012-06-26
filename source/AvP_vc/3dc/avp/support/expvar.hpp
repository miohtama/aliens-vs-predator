/*
	
	expvar.hpp

	Template for variable with pure virtual get/set methods
	(I believe Borland's Delphi calls them "properties")

	Handy for setting up e.g. selection items in the menu system

	I've called them ExportVariables

*/

#ifndef _expvar_hpp
#define _expvar_hpp 1

	#if ( defined( __WATCOMC__ ) || defined( _MSC_VER ) )
		#pragma once
	#endif

#ifdef __cplusplus
#endif // __cplusplus

/* Type definitions *****************************************************/
template <class T> class ExportVariable
{
protected:
	ExportVariable()
	{
	}

public:	
	virtual T Get(void) const = 0;
	virtual void Set(T NewVal) = 0;

};

// Similar to an ExportVariable, but has boundary values defined.
// Can only be instantiated for types with ordering
template <class T> class BoundedExportVariable
{
protected:
	BoundedExportVariable
	(
		T minVal_New,
		T maxVal_New
	) :	minVal(minVal_New),
		maxVal(maxVal_New)
	{
	}

private:
	// This virtual function is private; it's only accessible within this class
	// although it must be overridden by derived classes
	virtual void Implement_Set(T NewVal) = 0;

public:	
	virtual T Get(void) const = 0;

	T GetMin(void) const
	{
		return minVal;
	}
	T GetMax(void) const
	{
		return maxVal;
	}
	void Set(T NewVal)
	{
		// Check bounds, reject if outside.  Otherwise call pure virtual fn
		// (which doesn't need to bother checking)
		if (NewVal<minVal) return;
		if (NewVal>maxVal) return;
		Implement_Set(NewVal);
	}


private:
	const T minVal;
	const T maxVal;
};

// An export var with empty "get/""set" hooks:
template <class T> class SimpleExportVariable : public ExportVariable<T>
{
public:
	SimpleExportVariable
	(
		T& aT
	) : theT(aT),
		ExportVariable()
	{
	}

	T Get(void) const
	{
		return theT;
	}
	void Set(T NewVal)
	{
		theT = NewVal;
	}	

protected:
	T& theT;

};

// A bounded export var with empty "get/""set" hooks:
template <class T> class SimpleBoundedExportVariable : public BoundedExportVariable<T>
{
public:
	SimpleBoundedExportVariable
	(
		T& aT,
		T minVal_New,
		T maxVal_New
	) : theT(aT),
		BoundedExportVariable
		(
			minVal_New,
			maxVal_New
		)
	{
	}

	T Get(void) const
	{
		return theT;
	}
	
	void Implement_Set(T NewVal)
	{
		theT = NewVal;
	}

private:
	T& theT;


};

/* End of the header ****************************************************/



#endif
