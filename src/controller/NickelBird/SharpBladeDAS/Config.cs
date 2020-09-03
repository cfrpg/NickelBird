using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using System.ComponentModel;

namespace SharpBladeDAS
{
	[Serializable]
	public class Config : INotifyPropertyChanged
	{
		bool[] logEnabled;
		bool[] filterEnabled;
		double[] offsets;
		double[] scales;		
		string[] names;
		double[] factors;

		string logPath;
		string matrixName;
		string configName;
		
		double filterFc;
		double filterFs;
		int filterOrder;
		int logSkip;
			
		int[] plotData;
		int[] tableData;

		string[] logName;
		string[] logNameBackup;

		bool isCsv;

		public event PropertyChangedEventHandler PropertyChanged;

		public event EventHandler OnFilterParamChanged;

		public bool[] LogEnabled { get => logEnabled; set => logEnabled = value; }
		public double[] Offsets { get => offsets; set => offsets = value; }
		public double[] Scales { get => scales; set => scales = value; }
		public string LogPath
		{
			get => logPath;
			set
			{
				logPath = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("LogPath"));
			}
		}

		public string[] Names 
		{ 
			get => names;
			set 
			{
				names = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Names"));
			}
		}
	

		public bool[] FilterEnabled { get => filterEnabled; set => filterEnabled = value; }
		
		public double[] Factors { get => factors; set => factors = value; }
		
		public double FilterFs
		{
			get => filterFs;
			set
			{
				filterFs = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("FilterFs"));
				OnFilterParamChanged?.Invoke(this, new EventArgs());
			}
		}
		public double FilterFc
		{
			get => filterFc;
			set
			{
				filterFc = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("FilterFc"));
				OnFilterParamChanged?.Invoke(this, new EventArgs());
			}
		}
		public int FilterOrder
		{
			get => filterOrder;
			set
			{
				filterOrder = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("FilterOrder"));
				OnFilterParamChanged?.Invoke(this, new EventArgs());
			}
		}

		

		public int[] PlotData { get => plotData; set => plotData = value; }
		//public double[] PlotDataScale { get => plotDataScale; set => plotDataScale = value; }
		//public string[] PlotDataName { get => plotDataName; set => plotDataName = value; }
		public string[] LogName { get => logName; set => logName = value; }
		public string[] LogNameBackup { get => logNameBackup; set => logNameBackup = value; }
		public bool IsCsv
		{
			get => isCsv;
			set
			{
				isCsv = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsCsv"));
			}
		}

		public string MatrixName 
		{ 
			get => matrixName;
			set
			{
				matrixName = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("MatrixName"));
			}
		}

		public string ConfigName
		{
			get => configName;
			set
			{
				configName = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("ConfigName"));
			}
		}

		public int LogSkip
		{
			get => logSkip;
			set
			{
				logSkip = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("LogSkip"));
			}
		}

		public int[] TableData { get => tableData; set => tableData = value; }

		public static Config Default(int ch)
		{
			bool[] b = new bool[ch];
			double[] o = new double[ch];
			double[] s = new double[ch];
			string[] n = new string[ch];			
			bool[] f = new bool[ch];
			double[] fa = new double[ch];
			int[] pd = new int[] { 14, 15, 16, 17, 18, 19 };
			//double[] pds = new double[] { 1000, 1000, 1000, 1 };
			//string[] pdn = new string[] { "Fy", "Fx", "M", "I" };
			for (int i = 0; i < ch; i++)
			{
				b[i] = true;
				f[i] = false;
				
				o[i] = 0;
				s[i] = 1;
				n[i] = "Sensor " + i.ToString();
				if (i < 8)
					fa[i] = 10.0 / 65535;
				else if (i < 10)
					fa[i] = 3.3 / 4096;
				else
					fa[i] = 1;
				//fa[i] = i < 8 ? 10.0 / 65535 : 3.3 / 4096;
			}
			return new Config() {
				LogEnabled = b,
				FilterEnabled = f,
				FilterFc = 25,
				FilterFs = 1000,
				FilterOrder = 5,
				Scales = s,
				Offsets = o,
				LogPath = System.Environment.CurrentDirectory + "\\Logs",
				//MatrixPath = System.Environment.CurrentDirectory + "\\Matrices\\cailMatrix.csv",
				MatrixName = "",
				ConfigName = "config.xml",
				Names = n,
				Factors = fa,
				PlotData = pd,
				//PlotDataScale = pds,
				//PlotDataName = pdn,
				LogName = new string[] { "1", "2", "3", "4", "5" },
				LogNameBackup = new string[6],
				isCsv = true,
				LogSkip = 1,
				TableData = new int[] { 8, 10 }
			};

		}
	}
}
