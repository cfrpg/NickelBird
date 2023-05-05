using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace SharpBladeDAS
{
	public class SensorData : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;
		int id;
		double value;
		double offset;
		double scale;
		bool logEnabled;
		double scaledValue;
		double lpfValue;
		double rawValue;

		bool filterEnabled;				

		string name;

		ButterworthLPF filter;

		Config config;

		double factor;

		SyncFlag syncFlag;

		private static double[] rangeFactor = new double[8] { 2.5 * 2 / 65535, 5.0 * 2 / 65535, 6.25 * 2 / 65535, 10.0 * 2 / 65535, 12.5 * 2 / 65535, 20.0 * 2 / 65535, 5.0 / 65535, 10.0 / 65535 };

		public double Value
		{
			get => value;
			set
			{
				rawValue = value;
				this.value = value;
				scaledValue = (this.value + offset) * scale;				
				if (filterEnabled)
				{
					if (Filter != null)
					{
						LpfValue = Filter.Process(scaledValue);
					}
					else
						LpfValue = ScaledValue;
				}
				else
				{
					LpfValue = ScaledValue;
				}
				if (syncFlag.UpdateUI)
				{
					PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("ScaledValue"));
					PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Value"));
					PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("LpfValue"));
				}
			}

		}
		public double Offset
		{
			get => offset;
			set
			{
				offset = value;
				scaledValue = (this.value + offset) * scale;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("ScaledValue"));
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Offset"));
			}
		}
		public double Scale
		{
			get => scale;
			set
			{
				scale = value;
				scaledValue = (this.value + offset) * scale;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("ScaledValue"));
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Scale"));
			}
		}
		public bool LogEnabled
		{
			get => logEnabled;
			set
			{
				logEnabled = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("LogEnabled"));
			}
		}

		public double ScaledValue { get => scaledValue; private set => scaledValue = value; }
		public string Name
		{
			get => name;
			set
			{
				name = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Name"));
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("FullName"));
			}
		}

		public string FullName
		{
			get
			{
				return id.ToString() + " : " + name;
			}
		}
		public bool FilterEnabled
		{
			get => filterEnabled;
			set
			{
				filterEnabled = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("FilterEnabled"));
			}
		}
				
		public double LpfValue { get => lpfValue; set => lpfValue = value; }
		public Config Config { get => config; set => config = value; }
		public double RawValue { get => rawValue; set => rawValue = value; }
		
		public double Factor { get => factor; set => factor = value; }
		public SyncFlag SyncFlag { get => syncFlag; set => syncFlag = value; }
		public Filter Filter { get => filter; }
		public int Id { get => id; set => id = value; }
		public static double[] RangeFactor { get => rangeFactor; }

		public SensorData(SyncFlag sf,Config c)
		{
			value = 0;
			offset = 0;
			scale = 1;
			logEnabled = true;
			name = "Sensor";
			syncFlag = sf;
			config = c;
			filter = new ButterworthLPF(config.FilterOrder, config.FilterFc, config.FilterFs);
			config.OnFilterParamChanged += Config_OnFilterParamChanged;
		}

		private void Config_OnFilterParamChanged(object sender, EventArgs e)
		{
			filter.Order = config.FilterOrder;
			filter.CutoffFrequency = config.FilterFc;
			filter.SimplingFrequency = config.FilterFs;
			filter.Generate();
		}

		public void SetRawValue(int v)
		{
			Value = v * factor;
		}

		public void SetRawValue(float v)
		{
			Value = v * factor;
		}
	}
}
