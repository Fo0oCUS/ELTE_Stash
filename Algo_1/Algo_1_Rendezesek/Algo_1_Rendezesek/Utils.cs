using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Algo_1_Rendezesek
{
    public static class Utils<T>
    {
        public static void Swap(ref List<T> data, int i, int j)
        {
            (data[j], data[i]) = (data[i], data[j]);
        }
    }
}
