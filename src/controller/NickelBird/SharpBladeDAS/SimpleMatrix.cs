using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace SharpBladeDAS
{
	public class SimpleMatrix
	{
		double[,] data;

		int row;
		int col;
		public SimpleMatrix(int r):this(r,r,true)
		{

		}

		public SimpleMatrix(int r,int c,bool log)
		{
			row = r;
			col = c;
			data = new double[r, c+1];
			for(int i=0;i<r;i++)
			{
				for (int j = 0; j < c; j++)
				{
					if (i == j && !log)
						data[i, j] = 1;
					else
						data[i, j] = 0;
				}
			}
			for (int i = 0; i < row; i++)
			{
				data[i, c] = (log ? 1 : 0);
			}
			
		}
		
		public int Row { get => row; }
		public int Col { get => col; }
		public double[,] Data { get => data; set => data = value; }
				

		public void Transform(List<double> source,int sourceOffset,List<double> dest,int destOffset)
		{
			double ans;
			for (int i = 0; i < row; i++)
			{
				ans = 0;
				for (int j = 0; j < col; j++)
				{
					if (double.IsInfinity(source[sourceOffset + j]) || double.IsNaN(source[sourceOffset + j]))
						continue;
					ans += source[sourceOffset + j] * data[i, j];
				}
				dest[i + destOffset] += ans + data[i, col];			
			}
		}

		public void TransformLog(List<double> source, int sourceOffset, List<double> dest, int destOffset)
		{
			double ans;
			for (int i = 0; i < row; i++)
			{
				ans = 1;
				for (int j = 0; j < col; j++)
				{
					if (data[i, j] == 0)
						continue;
					if (double.IsInfinity(source[sourceOffset + j]) || double.IsNaN(source[sourceOffset + j]))
						continue;
					ans *= Math.Pow(source[sourceOffset + j] , data[i, j]);
				}
				dest[i + destOffset] *= ans * data[i, col];
			}
		}
	}
}
