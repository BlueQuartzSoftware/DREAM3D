#ifndef _CellularAutomataHelpers_H_
#define _CellularAutomataHelpers_H_

namespace CellularAutomata
{
	//helper class to get neighbors with periodic boundary conditions
	class Lattice
	{
		size_t dims[3];
		size_t yDim;
		size_t zDim;
		size_t sliceSize;
		size_t numCells;

		//function to get directional neighbors with periodic boundary conditions
		inline size_t next(size_t i, size_t direction)
		{
			i++;
			if(dims[direction] == i)
				return 0;
			return i;
		}

		inline size_t prev(size_t i, size_t direction)
		{
			if(0 == i)
				i = dims[direction];
			i--;
			return i;
		}

	public:
		Lattice(size_t x, size_t y, size_t z)
		{
			dims[0] = x;
			dims[1] = y;
			dims[2] = z;
			sliceSize = x * y;
			numCells = sliceSize * z;
		}

		size_t size()
		{
			return dims[0] * dims[1] * dims[2];
		}

		//given an (x,y,z) tuple compute the index
		inline size_t ToIndex(size_t x, size_t y, size_t z)
		{
			return z * sliceSize + y * dims[0] + x;
		}

		//given an index compute the (x,y,z) tuple
		inline void ToTuple(size_t index, size_t& x, size_t& y, size_t& z)
		{
			z = index / sliceSize;
			index -= z * sliceSize;
			y = index / dims[0];
			x = index - y * dims[0];
		}

		//given an (x, y, z) tuple and neighbor offset, compute the corresponding neighbor index (with periodic boundary conditions)
		inline size_t operator() (int x, int y, int z, int dx, int dy, int dz)
		{
			//get neighbor indicies
			int neighborX = x + dx;
			int neighborY = y + dy;
			int neighborZ = z + dz;

			//apply periodic boundary conditions
			while(neighborX < 0)
				neighborX += dims[0];
			while(neighborX >= dims[0])
				neighborX -= dims[0];

			while(neighborY < 0)
				neighborY += dims[0];
			while(neighborY >= dims[0])
				neighborY -= dims[0];

			while(neighborZ < 0)
				neighborZ += dims[0];
			while(neighborZ >= dims[0])
				neighborZ -= dims[0];

			//convert to index + return
			return this->ToIndex(neighborX, neighborY, neighborZ);
		}

		//given an index and offset, compute the corresponding neighbor index (with periodic boundary conditions)
		inline size_t operator() (size_t index, int dx, int dy, int dz)
		{
			//convert index to tuple
			size_t x, y, z;
			this->ToTuple(index, x, y, z);

			//return neighbor index
			return (x, y, z, dx, dy, dz);
		}
		
		/*
		 * Functions to get the neighhors of a pixel
		 */

		 	//6/face connected
			std::vector<size_t> VonNeumann(size_t x, size_t y, size_t z)
			{
				//get neighbor indicies
				size_t xPrev = prev(x, 0);
				size_t xNext = next(x, 0);
				size_t yPrev = prev(y, 1);
				size_t yNext = next(y, 1);
				size_t zPrev = prev(z, 2);
				size_t zNext = next(z, 2);

				//compute neigbors
				std::vector<size_t> neighbors(6, 0);
				neighbors[0] = ToIndex(xPrev, y, z);
				neighbors[1] = ToIndex(xNext, y, z);
				neighbors[2] = ToIndex(x, yPrev, z);
				neighbors[3] = ToIndex(x, yNext, z);
				neighbors[4] = ToIndex(x, y, zPrev);
				neighbors[5] = ToIndex(x, y, zNext);
				return neighbors;
			}
			std::vector<size_t> VonNeumann(size_t index)
			{
				size_t x, y, z;
				ToTuple(index, x, y, z);
				return VonNeumann(x, y, z);
			}

			//18/face+edge connected
			std::vector<size_t> EighteenCell(size_t x, size_t y, size_t z)
			{
				//get neighbor indicies
				size_t xPrev = prev(x, 0);
				size_t xNext = next(x, 0);
				size_t yPrev = prev(y, 1);
				size_t yNext = next(y, 1);
				size_t zPrev = prev(z, 2);
				size_t zNext = next(z, 2);

				//compute neigbors
				std::vector<size_t> neighbors(18, 0);

				//faces
				neighbors[0] = ToIndex(xPrev, y, z);
				neighbors[1] = ToIndex(xNext, y, z);
				neighbors[2] = ToIndex(x, yPrev, z);
				neighbors[3] = ToIndex(x, yNext, z);
				neighbors[4] = ToIndex(x, y, zPrev);
				neighbors[5] = ToIndex(x, y, zNext);

				//edges
				neighbors[6] = ToIndex(x, yPrev, zPrev);
				neighbors[7] = ToIndex(x, yPrev, zNext);
				neighbors[8] = ToIndex(x, yNext, zPrev);
				neighbors[9] = ToIndex(x, yNext, zNext);

				neighbors[10] = ToIndex(xPrev, y, zPrev);
				neighbors[11] = ToIndex(xPrev, y, zNext);
				neighbors[12] = ToIndex(xNext, y, zPrev);
				neighbors[13] = ToIndex(xNext, y, zNext);

				neighbors[14] = ToIndex(xPrev, yPrev, z);
				neighbors[15] = ToIndex(xPrev, yNext, z);
				neighbors[16] = ToIndex(xNext, yPrev, z);
				neighbors[17] = ToIndex(xNext, yNext, z);
				return neighbors;
			}
			std::vector<size_t> EighteenCell(size_t index)
			{
				size_t x, y, z;
				ToTuple(index, x, y, z);
				return EighteenCell(x, y, z);
			}

			//26/face+edge+corner connected
			std::vector<size_t> Moore(size_t x, size_t y, size_t z)
			{
				//get neighbor indicies
				size_t xPrev = prev(x, 0);
				size_t xNext = next(x, 0);
				size_t yPrev = prev(y, 1);
				size_t yNext = next(y, 1);
				size_t zPrev = prev(z, 2);
				size_t zNext = next(z, 2);

				//compute neigbors
				std::vector<size_t> neighbors(26, 0);

				//faces
				neighbors[0] = ToIndex(xPrev, y, z);
				neighbors[1] = ToIndex(xNext, y, z);
				neighbors[2] = ToIndex(x, yPrev, z);
				neighbors[3] = ToIndex(x, yNext, z);
				neighbors[4] = ToIndex(x, y, zPrev);
				neighbors[5] = ToIndex(x, y, zNext);

				//edges
				neighbors[6] = ToIndex(x, yPrev, zPrev);
				neighbors[7] = ToIndex(x, yPrev, zNext);
				neighbors[8] = ToIndex(x, yNext, zPrev);
				neighbors[9] = ToIndex(x, yNext, zNext);

				neighbors[10] = ToIndex(xPrev, y, zPrev);
				neighbors[11] = ToIndex(xPrev, y, zNext);
				neighbors[12] = ToIndex(xNext, y, zPrev);
				neighbors[13] = ToIndex(xNext, y, zNext);

				neighbors[14] = ToIndex(xPrev, yPrev, z);
				neighbors[15] = ToIndex(xPrev, yNext, z);
				neighbors[16] = ToIndex(xNext, yPrev, z);
				neighbors[17] = ToIndex(xNext, yNext, z);

				//corners
				neighbors[18] = ToIndex(xPrev, yPrev, zPrev);
				neighbors[19] = ToIndex(xPrev, yPrev, zNext);
				neighbors[20] = ToIndex(xPrev, yNext, zPrev);
				neighbors[21] = ToIndex(xPrev, yNext, zNext);

				neighbors[22] = ToIndex(xNext, yPrev, zPrev);
				neighbors[23] = ToIndex(xNext, yPrev, zNext);
				neighbors[24] = ToIndex(xNext, yNext, zPrev);
				neighbors[25] = ToIndex(xNext, yNext, zNext);
				return neighbors;
			}
			std::vector<size_t> Moore(size_t index)
			{
				size_t x, y, z;
				ToTuple(index, x, y, z);
				return Moore(x, y, z);
			}

			//2 shells of 26 connectivity (26 connected neighborhood of all 26 connected neighbors)
			std::vector<size_t> ExtendedMoore(size_t x, size_t y, size_t z)
			{
				//get neighbor indicies
				size_t xIndicies[5];
				size_t yIndicies[5];
				size_t zIndicies[5];

				xIndicies[1] = prev(x, 0);
				xIndicies[0] = prev(xIndicies[1], 0);
				xIndicies[2] = x;
				xIndicies[3] = next(x, 0);
				xIndicies[4] = next(xIndicies[3], 0);

				yIndicies[1] = prev(y, 0);
				yIndicies[0] = prev(yIndicies[1], 0);
				yIndicies[2] = y;
				yIndicies[3] = next(y, 0);
				yIndicies[4] = next(yIndicies[3], 0);

				zIndicies[1] = prev(z, 0);
				zIndicies[0] = prev(zIndicies[1], 0);
				zIndicies[2] = z;
				zIndicies[3] = next(z, 0);
				zIndicies[4] = next(zIndicies[3], 0);

				//compute neigbors
				std::vector<size_t> neighbors;
				neighbors.reserve(124);

				//get all neighbors
				for(size_t i = 0; i < 5; i++)
				{
					for(size_t j = 0; j < 5; j++)
					{
						for(size_t k = 0; k < 5; k++)
						{
							if(i == 2 && j == 2 && k == 2)
								continue;
							neighbors.push_back( ToIndex(xIndicies[i], yIndicies[j], zIndicies[k]) );
						}
					}
				}				
				return neighbors;
			}
			std::vector<size_t> ExtendedMoore(size_t index)
			{
				size_t x, y, z;
				ToTuple(index, x, y, z);
				return ExtendedMoore(x, y, z);
			}

			//face connected + 2 opposing edge connected (~spherical)
			//Eight cell has 6 variants in 3d (6 opposing pairs of edges)
			std::vector<size_t> EightCell(size_t x, size_t y, size_t z, size_t variant)
			{
				//get neighbor indicies
				size_t xPrev = prev(x, 0);
				size_t xNext = next(x, 0);
				size_t yPrev = prev(y, 1);
				size_t yNext = next(y, 1);
				size_t zPrev = prev(z, 2);
				size_t zNext = next(z, 2);

				//compute neigbors
				std::vector<size_t> neighbors(8, 0);

				//always has faces
				neighbors[0] = ToIndex(xPrev, y, z);
				neighbors[1] = ToIndex(xNext, y, z);
				neighbors[2] = ToIndex(x, yPrev, z);
				neighbors[3] = ToIndex(x, yNext, z);
				neighbors[4] = ToIndex(x, y, zPrev);
				neighbors[5] = ToIndex(x, y, zNext);

				//edges
				switch(variant)
				{
					case 0:
						neighbors[6] = ToIndex(xPrev, yPrev, z);
						neighbors[7] = ToIndex(xNext, yNext, z);
						break;

					case 1:
						neighbors[6] = ToIndex(xPrev, yNext, z);
						neighbors[7] = ToIndex(xNext, yPrev, z);
						break;

					case 2:
						neighbors[6] = ToIndex(xPrev, y, zPrev);
						neighbors[7] = ToIndex(xNext, y, zNext);
						break;

					case 3:
						neighbors[6] = ToIndex(xPrev, y, zNext);
						neighbors[7] = ToIndex(xNext, y, zPrev);
						break;

					case 4:
						neighbors[6] = ToIndex(x, yPrev, zPrev);
						neighbors[7] = ToIndex(x, yNext, zNext);
						break;

					case 5:
						neighbors[6] = ToIndex(x, yPrev, zNext);
						neighbors[7] = ToIndex(x, yNext, zPrev);
						break;
				}
				return neighbors;
			}
			std::vector<size_t> EightCell(size_t index, size_t variant)
			{
				size_t x, y, z;
				ToTuple(index, x, y, z);
				return EightCell(x, y, z, variant);
			}


			//6 connected + 2 opposing corner connected + adjacent edge connected
			//Fourteen cell has 4 variants in 3d (4 opposing pairs of corners)
			std::vector<size_t> FourteenCell(size_t x, size_t y, size_t z, size_t variant)
			{
				//get neighbor indicies
				size_t xPrev = prev(x, 0);
				size_t xNext = next(x, 0);
				size_t yPrev = prev(y, 1);
				size_t yNext = next(y, 1);
				size_t zPrev = prev(z, 2);
				size_t zNext = next(z, 2);

				//compute neigbors
				std::vector<size_t> neighbors(14, 0);

				//always has faces
				neighbors[0] = ToIndex(xPrev, y, z);
				neighbors[1] = ToIndex(xNext, y, z);
				neighbors[2] = ToIndex(x, yPrev, z);
				neighbors[3] = ToIndex(x, yNext, z);
				neighbors[4] = ToIndex(x, y, zPrev);
				neighbors[5] = ToIndex(x, y, zNext);

				//corners + adjacent edges
				switch(variant)
				{
					case 0:
						neighbors[6] = ToIndex(xPrev, yPrev, zPrev);
						neighbors[7] = ToIndex(x, yPrev, zPrev);
						neighbors[8] = ToIndex(xPrev, y, zPrev);
						neighbors[9] = ToIndex(xPrev, yPrev, z);
						neighbors[10] = ToIndex(xNext, yNext, zNext);
						neighbors[11] = ToIndex(x, yNext, zNext);
						neighbors[12] = ToIndex(xNext, y, zNext);
						neighbors[13] = ToIndex(xNext, yNext, z);
						break;

					case 1:
						neighbors[6] = ToIndex(xPrev, yPrev, zNext);
						neighbors[7] = ToIndex(x, yPrev, zNext);
						neighbors[8] = ToIndex(xPrev, y, zNext);
						neighbors[9] = ToIndex(xPrev, yPrev, z);
						neighbors[10] = ToIndex(xNext, yNext, zPrev);
						neighbors[11] = ToIndex(x, yNext, zPrev);
						neighbors[12] = ToIndex(xNext, y, zPrev);
						neighbors[13] = ToIndex(xNext, yNext, z);
						break;

					case 2:
						neighbors[6] = ToIndex(xPrev, yNext, zPrev);
						neighbors[7] = ToIndex(x, yNext, zPrev);
						neighbors[8] = ToIndex(xPrev, y, zPrev);
						neighbors[9] = ToIndex(xPrev, yNext, z);
						neighbors[10] = ToIndex(xNext, yPrev, zNext);
						neighbors[11] = ToIndex(x, yPrev, zNext);
						neighbors[12] = ToIndex(xNext, y, zNext);
						neighbors[13] = ToIndex(xNext, yPrev, z);
						break;

					case 3:
						neighbors[6] = ToIndex(xPrev, yNext, zNext);
						neighbors[7] = ToIndex(x, yNext, zNext);
						neighbors[8] = ToIndex(xPrev, y, zNext);
						neighbors[9] = ToIndex(xPrev, yNext, z);
						neighbors[10] = ToIndex(xNext, yPrev, zPrev);
						neighbors[11] = ToIndex(x, yPrev, zPrev);
						neighbors[12] = ToIndex(xNext, y, zPrev);
						neighbors[13] = ToIndex(xNext, yPrev, z);
						break;
				}				
				return neighbors;
			}
			std::vector<size_t> FourteenCell(size_t index, size_t variant)
			{
				size_t x, y, z;
				ToTuple(index, x, y, z);
				return FourteenCell(x, y, z, variant);
			}

			//face connected + edge connected + 2 opposing corner connected
			//Twnety cell has 4 variants in 3d (4 opposing pairs of corners)
			std::vector<size_t> TwentyCell(size_t x, size_t y, size_t z, size_t variant)
			{
				//get neighbor indicies
				size_t xPrev = prev(x, 0);
				size_t xNext = next(x, 0);
				size_t yPrev = prev(y, 1);
				size_t yNext = next(y, 1);
				size_t zPrev = prev(z, 2);
				size_t zNext = next(z, 2);

				//compute neigbors
				std::vector<size_t> neighbors(20, 0);

				//always has faces
				neighbors[0] = ToIndex(xPrev, y, z);
				neighbors[1] = ToIndex(xNext, y, z);
				neighbors[2] = ToIndex(x, yPrev, z);
				neighbors[3] = ToIndex(x, yNext, z);
				neighbors[4] = ToIndex(x, y, zPrev);
				neighbors[5] = ToIndex(x, y, zNext);

				//edges
				neighbors[6] = ToIndex(x, yPrev, zPrev);
				neighbors[7] = ToIndex(x, yPrev, zNext);
				neighbors[8] = ToIndex(x, yNext, zPrev);
				neighbors[9] = ToIndex(x, yNext, zNext);

				neighbors[10] = ToIndex(xPrev, y, zPrev);
				neighbors[11] = ToIndex(xPrev, y, zNext);
				neighbors[12] = ToIndex(xNext, y, zPrev);
				neighbors[13] = ToIndex(xNext, y, zNext);

				neighbors[14] = ToIndex(xPrev, yPrev, z);
				neighbors[15] = ToIndex(xPrev, yNext, z);
				neighbors[16] = ToIndex(xNext, yPrev, z);
				neighbors[17] = ToIndex(xNext, yNext, z);

				//corners
				switch(variant)
				{
					case 0:
						neighbors[18] = ToIndex(xPrev, yPrev, zPrev);
						neighbors[19] = ToIndex(xNext, yNext, zNext);
						break;

					case 1:
						neighbors[18] = ToIndex(xPrev, yPrev, zNext);
						neighbors[19] = ToIndex(xNext, yNext, zPrev);
						break;

					case 2:
						neighbors[18] = ToIndex(xPrev, yNext, zPrev);
						neighbors[19] = ToIndex(xNext, yPrev, zNext);
						break;

					case 3:
						neighbors[18] = ToIndex(xPrev, yNext, zNext);
						neighbors[19] = ToIndex(xNext, yPrev, zPrev);
						break;
				}
				
				return neighbors;
			}
			std::vector<size_t> TwentyCell(size_t index, size_t variant)
			{
				size_t x, y, z;
				ToTuple(index, x, y, z);
				return TwentyCell(x, y, z, variant);
			}
	};
}

#endif