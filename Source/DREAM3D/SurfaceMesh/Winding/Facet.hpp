/***
 * Name:	Facet.h
 * Purpose:	m3c.basics.Facet Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
 * Unported License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc/3.0/
 * or send a letter to Creative Commons,
 * 171 Second Street, Suite 300,
 * San Francisco, California, 94105, USA.
 ***/

#ifndef H_m3c_BASICS_TRIANGLE
#define H_m3c_BASICS_TRIANGLE

#include <vector>
#include <set>
#include <map>

#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "Edge.h"

namespace m3c
{

  template<typename Label>
  class Facet
  {
    public:
      MXA_SHARED_POINTERS(Facet<Label>)
      MXA_CONTAINER_TYPE(Pointer, std::vector)


      static Pointer New(int i0, int i1, int i2)
      {
        Pointer sharedPtr(new Facet<Label>(i0, i1, i2));
        return sharedPtr;
      }

      static Pointer New1(int i0, int i1, int i2)
      {
        Pointer sharedPtr(new Facet<Label>(i0, i1, i2));
        return sharedPtr;
      }

      static Pointer New2(int i0, int i1, int i2)
      {
        Pointer sharedPtr(new Facet<Label>(i0, i1, i2));
        return sharedPtr;
      }

      virtual ~Facet()
      {
      }

      //#################### PUBLIC VARIABLES ####################
      int verts[3];
      Label labels[2];
      m3c::Edge::Pointer edges[3];
      int tIndex;

      // -----------------------------------------------------------------------------
      //
      // -----------------------------------------------------------------------------
      void flipWinding()
      {
        int tmp = verts[0];
        verts[0] = verts[2];
        verts[2] = tmp;
      }
      // -----------------------------------------------------------------------------
      //
      // -----------------------------------------------------------------------------
      int getLabelIndex(Label label)
      {
        if (label == labels[0]) return 0;
        if (label == labels[1]) return 1;
        return 2; // Error condition. Valid values are 0 or 1 since there are only 2 elements to the array.
      }

      // -----------------------------------------------------------------------------
      //
      // -----------------------------------------------------------------------------
      std::vector<int> getNodeIndices(Label label)
      {
        std::vector<int> tNodes(3);
        int idx = getLabelIndex(label);
        if (idx == 1)
        {
          tNodes[0] = verts[2];
          tNodes[1] = verts[1];
          tNodes[2] = verts[0];
        }
        else
        {
          tNodes[0] = verts[0];
          tNodes[1] = verts[1];
          tNodes[2] = verts[2];
        }
        return tNodes;
      }

      // -----------------------------------------------------------------------------
      //
      // -----------------------------------------------------------------------------
      void getWindingIndices(int ids[3], Label label)
      {
        int idx = getLabelIndex(label);
        if (idx == 1)
        {
          ids[0] = verts[2];
          ids[1] = verts[1];
          ids[2] = verts[0];
        }
        else
        {
          ids[0] = verts[0];
          ids[1] = verts[1];
          ids[2] = verts[2];
        }
      }

      // -----------------------------------------------------------------------------
      //
      // -----------------------------------------------------------------------------
      void getWindingIndices4(int ids[4], Label label)
      {
        int idx = getLabelIndex(label);

        if (idx == 1)
        {
          ids[0] = verts[2];
          ids[1] = verts[1];
          ids[2] = verts[0];
          ids[3] = verts[2];
        }
        else
        {
          ids[0] = verts[0];
          ids[1] = verts[1];
          ids[2] = verts[2];
          ids[3] = verts[0];
        }
      }
      // -----------------------------------------------------------------------------
      //
      // -----------------------------------------------------------------------------
      void verifyWinding(Pointer tri, Label label)
      {
        int ids[4];
        int nids[4];
        getWindingIndices4(ids, label);
        tri->getWindingIndices4(nids, label);
        int idx = 0;
        // There are 2 verts that are shared between the two triangles
        // Find them
        int i0, i1;
        bool flip = false;
        bool done = false;
        for (int i = 0; i < 3; ++i)
        {
          i0 = ids[i];
          i1 = ids[i + 1];
          for (int j = 0; j < 3; ++j)
          {
            if (i0 == nids[j + 1] && i1 == nids[j])
            {
              //    std::cout << ">>>>>> Winding OK "<< tIndex << " <-> "<< tri->tIndex << std::endl;
              done = true;
              break;
            }
            else if (i0 == nids[j] && i1 == nids[j + 1])
            {
              std::cout << "!!!!!! Winding Bad " << tIndex << " <-> " << tri->tIndex << std::endl;
              std::cout << "  Grain ID: " << label << std::endl;
              std::cout << "  Triangle ID: " << tIndex << std::endl;
              std::cout << "  Neighbor ID: " << tri->tIndex << std::endl;
              std::cout << "  Vert IDs: " << i0 << " & " << i1 << std::endl;
              done = true;
              tri->flipWinding();

              break;
            }
          }
          if (done) break;
        }
      }

    protected:
      //#################### CONSTRUCTORS ####################
      Facet(int i0, int i1, int i2)
      {
        verts[0] = i0;
        verts[1] = i1;
        verts[2] = i2;
        tIndex = -1;
      }

    private:
      Facet(const Facet&); // Copy Constructor Not Implemented
      void operator=(const Facet&); // Operator '=' Not Implemented
  };

}

#endif
