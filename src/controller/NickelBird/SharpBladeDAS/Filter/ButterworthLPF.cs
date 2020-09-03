using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharpBladeDAS
{
	public class ButterworthLPF : Filter
	{
		int order;
		double cutoffFrequency;
		double simplingFrequency;
		double Wc;

		double[] num;
		double[] den;

		FilterQueue xs;
		FilterQueue ys;

		static double[,] butterPb = {
			{1.0,0,0,0,0,0,0,0,0,0},
			{1.0, 1.4142136, 0,0,0,0,0,0,0,0},
			{1.0, 2.0, 2.0, 0,0,0,0,0,0,0},
			{1.0, 2.6131259, 3.4142136, 2.6131259, 0,0,0,0,0,0},
			{1.0, 3.236068, 5.236068, 5.236068, 3.236068, 0,0,0,0,0},
			{1.0, 3.8637033, 7.4641016, 9.1416202, 7.4641016, 3.8637033, 0,0,0,0},
			{1.0, 4.4939592, 10.0978347, 14.5917939, 14.5917939, 10.0978347, 4.4939592, 0,0,0},
			{1.0, 5.1258309, 13.1370712, 21.8461510, 25.6883559, 21.8461510, 13.1370712, 5.1258309, 0,0},
			{1.0, 5.7587705, 16.5817187, 31.1634375, 41.9863857, 41.9863857, 31.1634375, 16.5817187, 5.7587705, 0},
			{1.0, 6.3924532, 20.4317291, 42.8020611, 64.8823963, 74.2334292, 64.8823963, 42.8020611, 20.4317291, 6.3924532}
		};

		public ButterworthLPF(int o,double fc,double fs)
		{
			xs = new FilterQueue(20);
			ys = new FilterQueue(20);
			order = o;
			cutoffFrequency = fc;
			simplingFrequency = fs;
			Generate();
		}

		public int Order
		{
			get => order;
			set
			{
				order = value;
				if (order > 10)
					order = 10;
				if (order < 1)
					order = 1;
				//Generate();
				OnPropertyChanged("Order");
			}
		}
		public double CutoffFrequency
		{
			get => cutoffFrequency;
			set
			{
				cutoffFrequency = value;
				//Generate();
				OnPropertyChanged("CutoffFrequency");
			}
		}

		public double SimplingFrequency
		{
			get => simplingFrequency;
			set
			{
				simplingFrequency = value;
				//Generate();
				OnPropertyChanged("SimplingFrequency");
			}
		}

		public override double Process(double value)
		{
			double res;
			if (ys.Count<order)
			{
				xs.Enqueue(value);
				res = num[0] * xs[xs.Count - 1];
				for(int i=1;i<xs.Count-1;i++)
				{
					res += num[i] * xs[xs.Count-1 - i];
					res -= den[i] * ys[ys.Count - i];
				}
				ys.Enqueue(res);
				return value;
			}
			//xs.Dequeue();
			xs.Enqueue(value);
			res = num[0] * xs[order];
			for(int i=1;i<=order;i++)
			{
				res += num[i] * xs[order - i];
				res -= den[i] * ys[order - i];
			}
			res /= den[0];
			ys.Dequeue();
			xs.Dequeue();
			ys.Enqueue(res);
			return res;
		}

		public void Generate()
		{
			int N = order;
			double[] b = new double[N + 1];
			num = new double[N + 1];
			den = new double[N + 1];
			//预畸Omgea_c=|tan(pi*fc/fs)|
			Wc = Math.Abs(Math.Tan(Math.PI * cutoffFrequency / simplingFrequency));

			//计算Ha(s)分母系数bi=ai*Wc^(N-i),bN=1
			for (int i = 0; i < N; i++)
			{
				b[i] = butterPb[N - 1,i] * Math.Pow(Wc, N - i);
			}
			b[N] = 1;

			//计算H(z)
			//num=Wc^N*(1+Z^-1)^N
			//den=Sum(bi(1+z^-1)^(N-i)*(1-z^-1)^i)
			for(int i=0;i<=N;i++)
			{
				List<double> tmp1 = MathHelper.Binomial(i, -1);
				List<double> tmp2 = MathHelper.Binomial(N - i);
				MathHelper.PolynomialMul(tmp1, tmp2);
				for(int j=0;j<=N;j++)
				{
					den[j] += b[i] * tmp1[j];
				}
				if (i == 0)
				{
					for (int j = 0; j <= N; j++)
					{
						num[j] = b[0] * tmp2[j];
					}
				}
			}

			//归一化
			for (int i = N; i >= 0; i--)
			{				
				num[i] /= den[0];
				den[i] /= den[0];
			}

			xs.Clear();
			ys.Clear();
		}
	}
}
