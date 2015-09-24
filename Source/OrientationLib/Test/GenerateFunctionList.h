#ifndef _GenerateFunctionList_H_
#define _GenerateFunctionList_H_


#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>


class GenerateFunctionList
{

  public:
    GenerateFunctionList(){}
  virtual ~GenerateFunctionList(){}

    typedef std::vector<int> EntryType;

    /**
     * @brief GenerateTable
     * @param n
     * @param k
     * @return
     */
    std::vector<EntryType> GeneratePermutationsOfCombinations(int n, int k)
    {
      m_Combinations.clear();
      m_Permutations.clear();

      combinations(n, k);
      for(int i = 0; i < m_Combinations.size(); i++)
      {
        permutation(m_Combinations[i].size(), m_Combinations[i]);
      }
      return m_Permutations;
    }


  protected:


    /**
     * @brief combinations
     * @param n
     * @param k
     */
    void combinations(int n, int k)
    {
      std::string bitmask(k, 1); // K leading 1's
      bitmask.resize(n, 0); // N-K trailing 0's

      do {
        std::vector<int> entry;
        for (int i = 0; i < n; ++i) // [0..N-1] integers
        {
          if (bitmask[i])
          {
            entry.push_back(i);
          }
        }
        m_Combinations.push_back(entry);

      } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }

    /**
     * @brief permutation
     * @param n
     * @param ch
     * @return
     */
    int permutation(int n, std::vector<int> ch)
    {
      int i,j;
      int temp;
      int N = ch.size();

      if(n==0) {
        std::vector<int> entry;

        for(j=N-1;j>=0;j--)
        {
          entry.push_back(ch[j]);
          //std::cout<<ch[j];
        }
//        std::cout<<std::endl;
        m_Permutations.push_back(entry);
        return 0;
      }
      for(i=0;i<n;i++){
        temp = ch[i];
        for(j=i+1;j<n;j++)
        { ch[j-1] = ch[j]; }
        ch[n-1] = temp;
        //shift
        permutation(n-1, ch);
        for(j=n-1;j>i;j--)
        { ch[j] = ch[j-1]; }
        ch[i] = temp;
        //and shift back agian
      }
      return 1;
    }


  private:
    std::vector<EntryType> m_Combinations;
    std::vector<EntryType> m_Permutations;

    GenerateFunctionList(const GenerateFunctionList&); // Copy Constructor Not Implemented
    void operator=(const GenerateFunctionList&); // Operator '=' Not Implemented

};

#endif /* _GenerateFunctionList_H_ */
