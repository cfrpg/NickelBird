﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharpBladeDAS
{
	public static class MathHelper
	{
		static List<List<double>> binomialCoefficients;

		/// <summary>
		/// 返回(1+x)^n的系数表，x低次在前
		/// </summary>
		/// <param name="n"></param>
		/// <returns></returns>
		public static List<double> Binomial(int n)
		{
			return Binomial(n, 1);
		}

		/// <summary>
		/// 返回(1+-x)^n的系数表，x低次在前
		/// </summary>
		/// <param name="n"></param>
		/// <param name="sign">x的正负号</param>
		/// <returns></returns>
		public static List<double> Binomial(int n, int sign)
		{
			if (binomialCoefficients == null)
			{
				binomialCoefficients = new List<List<double>>();
				binomialCoefficients.Add(new List<double>());
				binomialCoefficients.Add(new List<double>());
				binomialCoefficients[0].Add(1);
				binomialCoefficients[1].Add(1);
				binomialCoefficients[1].Add(1);
			}
			List<double> res = new List<double>();
			for (int i = 0; i <= n; i++)
			{
				if (binomialCoefficients.Count >= i + 1)
					continue;
				List<double> tmp = new List<double>();
				tmp.Add(1);
				for (int j = 0; j < i - 1; j++)
				{
					tmp.Add(binomialCoefficients[i - 1][j] + binomialCoefficients[i - 1][j + 1]);
				}
				tmp.Add(1);
				binomialCoefficients.Add(tmp);
			}

			for (int i = 0; i < binomialCoefficients[n].Count; i++)
			{
				res.Add(binomialCoefficients[n][i]);
			}

			if (sign < 0)
			{
				for (int i = 1; i < res.Count; i += 2)
				{
					res[i] *= -1;
				}
			}

			return res;
		}
		
		/// <summary>
		/// 多项式乘法，低次在前，p1*=p2
		/// </summary>
		/// <param name="p1"></param>
		/// <param name="p2"></param>
		public static void PolynomialMul(List<double> p1, List<double> p2)
		{
			double[] res = new double[p1.Count + p2.Count - 1];
			for (int i = 0; i < p1.Count; i++)
			{
				for (int j = 0; j < p2.Count; j++)
				{
					res[i + j] += p1[i] * p2[j];
				}
			}
			p1.Clear();
			for (int i = 0; i < res.Length; i++)
			{
				p1.Add(res[i]);
			}

		}

		private static List<double> corrBuff;
		public static double CorrFreq(List<double> val,int offset,int n)
		{
			if (corrBuff == null)
			{
				corrBuff = new List<double>(1024);
				for (int i = 0; i < 1024; i++)
					corrBuff.Add(0);
			}
			//calc corr
			double divider=1;
			double min = 2;
			int minpos = 0;
			double aver = 0;
			for(int i=0;i<n;i++)
			{
				aver += val[(i + offset) % val.Count];
			}
			aver /= n;
			for (int i=0; i < n;i++)
			{				
				double sum = 0;
				for (int j = i; j < n; j++)
				{
					sum += (val[(j + offset) % (val.Count)]-aver) * (val[(j - i + offset) % (val.Count)]-aver);
				}
				if(i==0)
				{
					divider = sum;
				}
				corrBuff[i] = sum / divider;
				if(corrBuff[i]<min)
				{
					min = corrBuff[i];
					minpos = i;
				}
			}
			double max = -2;
			int maxpos = 0;
			for (int i = minpos; i < n; i++)
			{
				if(max<corrBuff[i])
				{
					max = corrBuff[i];
					maxpos = i;
				}
			}
			return 1000.0 / maxpos;
		}		
	}
}
