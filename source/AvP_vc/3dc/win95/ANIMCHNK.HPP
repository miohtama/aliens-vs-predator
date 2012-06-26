#ifndef _animchnk_hpp
#define _animchnk_hpp
#include "chunk.hpp"
#include "Chnktype.hpp"

struct TEXANIM;


class Animation_Chunk : public Chunk
{
public :
	Animation_Chunk(Chunk_With_Children* parent,const char*,size_t);
	Animation_Chunk(Chunk_With_Children* parent);
	~Animation_Chunk();
	
	virtual BOOL output_chunk (HANDLE &hand);
	
	virtual size_t size_chunk();
	
	virtual void fill_data_block(char* data_start);

	int NumPolys; //with animation in this shape
	TEXANIM** AnimList;
	
};

#define txa_flag_nointerptofirst 0x80000000

struct FrameList
{
	~FrameList();
	FrameList(TEXANIM*);
	#if InterfaceEngine
	FrameList(TEXANIM* p,FrameList* templ);
	#endif
	FrameList(TEXANIM* p,FrameList* fl,int* conv);
	int Speed;
	int Flags;
	
	int NumFrames;
	int CurFrame;
	TEXANIM* parent;

	int* Textures;
	int* UVCoords;
	int spare1,spare2;

	
	#if InterfaceEngine
	void CopyToSID(int shape,int poly);
	void CopyFromSID(int shape,int poly);
	void AddFrame();
	void RemoveFrame();
	#endif
};

#define AnimFlag_NotPlaying 0x00000001
struct TEXANIM
{
	TEXANIM(TEXANIM*);
	TEXANIM();
	~TEXANIM();
	
	#if InterfaceEngine
	TEXANIM(int s,int p,int id);
	//construct a TEXANIM using templ as a template.
	TEXANIM(int s,int p,TEXANIM* templ);
	#endif
	int shape;
	int poly;
	int NumVerts;
	int ID;
	int NumSeq;//number of sequences
	int CurSeq;
	int AnimFlags;
	int Identifier;
	FrameList** Seq;

	#if InterfaceEngine
	void ChangeFrame(int newseq,int newframe);
	void AddSeq();
	void RemoveSeq();
	void CopySeq(int seq_num);
	#endif
	void CopyAnimData(TEXANIM* ta,int* conv);
};

#endif
