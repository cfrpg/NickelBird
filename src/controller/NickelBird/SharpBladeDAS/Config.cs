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
		List<bool> logEnabled;
		List<bool> filterEnabled;
		List<double> offsets;
		List<double> scales;
		List<string> names;
		List<double> factors;

		string logPath;
		string matrixName;
		string configName;

		double filterFc;
		double filterFs;
		int filterOrder;
		int logSkip;
		int baudrate;
	
		int[] plotData;
		int[] tableData;
		int freqData;

		string[] logName;
		string[] logNameBackup;

		bool isCsv;
		bool freqEnabled;

		public event PropertyChangedEventHandler PropertyChanged;

		public event EventHandler OnFilterParamChanged;

		public List<bool> LogEnabled { get => logEnabled; set => logEnabled = value; }
		public List<double> Offsets { get => offsets; set => offsets = value; }
		public List<double> Scales { get => scales; set => scales = value; }
		public string LogPath
		{
			get => logPath;
			set
			{
				logPath = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("LogPath"));
			}
		}

		public List<string> Names 
		{
			get => names;
			set
			{
				names = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Names"));
			}
		}


		public List<bool> FilterEnabled { get => filterEnabled; set => filterEnabled = value; }

		public List<double> Factors { get => factors; set => factors = value; }

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
		public int Baudrate 
		{
			get => baudrate;
			set
			{
				baudrate = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Baudrate"));
			}
		}

		public bool FreqEnabled
		{
			get => freqEnabled;
			set
			{
				freqEnabled = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("FreqEnabled"));
			}
		}
		public int FreqData
		{
			get => freqData;
			set
			{
				freqData = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("FreqData"));
			}
		}

		public static Config Default(int ch)
		{
			List<bool> b = new List<bool>(ch);
			List<double> o = new List<double>(ch);
			List<double> s = new List<double>(ch);
			List<string> n = new List<string>(ch);
			List<bool> f = new List<bool>(ch);
			List<double> fa = new List<double>(ch);
			int[] pd = new int[] { 0, 1, 2, 3, 4, 5 };
			for (int i = 0; i < ch; i++)
			{
				b[i] = true;
				f[i] = false;

				o[i] = 0;
				s[i] = 1;
				n[i] = "Sensor " + i.ToString();
				if (i < 8)
					fa[i] = 10.0 / 65536;
				else if (i < 12)
					fa[i] = 3.3 / 4096;
				else
					fa[i] = 1;
				//fa[i] = i < 8 ? 10.0 / 65535 : 3.3 / 4096;
			}
			return new Config()
			{
				Baudrate = 2000000,
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
				TableData = new int[] { 0, 1 },
				FreqData = 0,
				FreqEnabled = false
			};

		}
	}
}
