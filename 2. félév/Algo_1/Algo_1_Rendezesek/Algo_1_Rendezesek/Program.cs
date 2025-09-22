namespace Algo_1_Rendezesek
{
    internal class Program
    {
        static void Main(string[] args)
        {
            List<double> data = new List<double>() { 34, 32, 25, 18, 43, 38, 32};
            Console.WriteLine("Before Sort: \n" + string.Join(", ", data));
            Sort<double>.MaxSort(ref data);
            Console.WriteLine("After Sort: \n" + string.Join(", ", data));
        }
    }
}
