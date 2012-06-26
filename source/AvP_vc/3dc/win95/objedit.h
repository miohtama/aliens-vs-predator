
#include "Chunk.hpp"

#define VECTOR VECTORCH

struct	ChunkMapBlock
{
	char TemplateName[20];
	char TemplateNotes[100];
	int MapType;
	int MapShape;
	int MapFlags;
	int MapFlags2;
	int MapFlags3;
	int MapCType;
	int MapCGameType;
	int MapCStrategyS;
	int MapCStrategyL;
	int MapInteriorType;
	int MapLightType;
	int MapMass;
	VECTOR MapNewtonV;
	VECTOR MapOrigin;
	int MapViewType;

	int MapVDBData;
	int SimShapeList;
	


};

class Map_Block_Chunk : public Chunk
{
public:
	virtual size_t size_chunk()
	{
		return (chunk_size=216);
	}
	virtual BOOL output_chunk (HANDLE &);

	virtual void fill_data_block (char * data_start);

	ChunkMapBlock map_data;
	friend class Object_Project_Data_Chunk;
	
	Map_Block_Chunk (Object_Project_Data_Chunk * parent)
		:Chunk(parent,"MAPBLOCK")
	{}
private:

	//constructor from buffer
	Map_Block_Chunk (Object_Project_Data_Chunk * parent,const char* data);
};

struct ChunkStrategy
{
	char StrategyName[20];
	char StrategyNotes[100];
	int Strategy;
};

class Strategy_Chunk : public Chunk
{
public :
	virtual size_t size_chunk()
	{
		return (chunk_size=136);
	}
	virtual BOOL output_chunk (HANDLE &);

	virtual void fill_data_block (char * data_start);

	ChunkStrategy strategy_data;
	friend class Object_Project_Data_Chunk;

	Strategy_Chunk(Object_Project_Data_Chunk *parent)
		:Chunk(parent,"STRATEGY")
	{}
private:

	//constructor from buffer
	Strategy_Chunk (Object_Project_Data_Chunk * parent,const char* data);
};
