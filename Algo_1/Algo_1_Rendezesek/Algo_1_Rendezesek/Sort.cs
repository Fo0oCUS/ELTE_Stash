using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Algo_1_Rendezesek
{
    public static class Sort<T> where T : IComparable<T>
    {
        public static void BubbleSort(ref List<T> data)
        {
            // BubbleSort(A : T[n])
            //   i := n - 1 downto 1
            //        j := 0 to i - 1
            //           A[j] > A[j + 1]
            //               Swap(A[j], A[j + 1])       |       Skip

            //  [12, 3, 5, 6, 1]
            //  [3, 5, 6, 1, 12]
            //  [3, 5, 1, 6, 12]
            //  [3, 1, 5, 6, 12]
            //  [1, 3, 5, 6, 12]

            int n = data.Count;
            for (int i = n - 1; i >= 1; i--)
            {
                for (int j = 0; j <= i - 1; j++)
                {
                    if (data[j].CompareTo(data[j + 1]) > 0)
                    {
                        Utils<T>.Swap(ref data, j, j + 1);
                    }
                }
            }
        }

        public static void ImprovedBubbleSort(ref List<T> data)
        {
            //  ImprovedBubbleSort(A : T[n])
            //      i := n - 1
            //      i >= 1
            //          u := -1
            //          j := 0 to i - 1
            //              A[j] > A[j + 1]
            //                  Swap(A[j], A[j + 1])        |       Skip
            //                  u := j                      |
            //          i := u

            int n = data.Count;
            int i = n - 1;
            while (i >= 1)
            {
                int u = -1;
                for (int j = 0; j <= i - 1; j++)
                {
                    if (data[j].CompareTo(data[j + 1]) > 0)
                    {
                        Utils<T>.Swap(ref data, j, j + 1);
                        u = j;
                    }
                }
                i = u;
            }
        }

        public static void InsertionSort(ref List<T> data)
        {
            // InsertionSort(A : T[n])
            //  i := 1 to n - 1
            //      A[i - 1] > A[i]
            //          x := A[i]                       |       
            //          A[i] := A[i - 1]                |
            //          j := i - 2                      |
            //          j >= 0 && A[j] > x              |           Skip
            //              A[j + 1] := A[j]            |
            //              j := j - 1                  |
            //          A[j + 1] := x                   |

            //  [80,46,39,28,64,29,66,62]
            //  [46,80,39,28,64,29,66,62]
            //  [39,46,80,28,64,29,66,62]
            //  [28,39,46,80,64,29,66,62]
            //  [28,39,46,64,80,29,66,62]
            //  [28,29,39,46,64,80,66,62]
            //  [28,29,39,46,64,66,80,62]
            //  [28,29,39,46,62,64,66,80]

            int n = data.Count;
            for (int i = 1; i <= n - 1; i++)
            {
                if (data[i - 1].CompareTo(data[i]) > 0)
                {
                    T x = data[i];
                    data[i] = data[i - 1];
                    int j = i - 2;
                    while (j >= 0 && data[j].CompareTo(x) > 0)
                    {
                        data[j + 1] = data[j];
                        j--;
                    }
                    data[j + 1] = x;
                }
            }
        }

        public static void MaxSort(ref List<T> data)
        {
            // MaxSort(A : T[n])
            //  i := n - 1 downto 1
            //      ind := 0
            //      j := 1 to i
            //          A[j] > A[ind]
            //              ind := j        |       Skip
            //      Swap(A[i], A[ind])

            //  [4,1,5,2,3]
            //  [4,1,3,2,5]
            //  [2,1,3,4,5]
            //  [2,1,3,4,5]
            //  [1,2,3,4,5]

            int n = data.Count;
            for (int i = n - 1; i >= 1; i--)
            {
                int ind = 0;
                for (int j = 1; j <= i; j++)
                {
                    if (data[j].CompareTo(data[ind]) > 0)
                    {
                        ind = j;
                    }
                }
                Utils<T>.Swap(ref data, i, ind);
            }
        }
    }
}
