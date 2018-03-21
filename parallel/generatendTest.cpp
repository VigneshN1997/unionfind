/*
This file is used for generating union find queries and also writing the final union find ds to file
*/

void generateRandomQueries(long int numQueries, long int maxNum)
{

	FILE* fp = fopen("queryFile_"+ltoa(numQueries),"w");
	const unsigned int range_from  = 0;
	const unsigned int range_to    = UINT_MAX;
	std::random_device                  rand_dev;
	std::mt19937                        generator(rand_dev());
	std::uniform_int_distribution<unsigned int>  distr(range_from, range_to);

	for(long int l = 0; l < numQueries; l++)
	{
		unsigned int x = distr(generator)%(uf->num_elems);
		unsigned int y = distr(generator)%(uf->num_elems);
		while(y == x)
		{
			y = distr(generator)%(uf->num_elems);
		}
		fprintf(fp, "%ld %ld\n",(long int)x,(long int)y);
	}
	fclose(fp);
}

void printUnionfindToFileMap(UnionFind* uf)
{
	
}


void printUnionfindToFileVector()
{

}