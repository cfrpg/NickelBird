using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Collections.ObjectModel;
using SharpBladeDAS;

namespace NickelBird
{
	public class FlightState : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		double airspeed;
		double airspeedkmh;
		double voltage;
		double freqency;
				

		ObservableCollection<double> graphData;
		ObservableCollection<double> tableData;
		ObservableCollection<double> graphMaxData;

		SyncFlag syncFlag;

		int ssTime;
		bool ssEanbled;

		double timer;

		bool sampleRateChanged;
		bool verisonChanged;
		int currentVerison;
		int sampleRate;
		ADCRange[] samplingRange;
		double sampleInterval;

		public double Airspeed
		{
			get => airspeed;
			set
			{
				airspeed = value;
				if(syncFlag.UpdateUI)
					PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Airspeed"));
			}
		}

		public double Airspeedkmh
		{
			get => airspeedkmh;
			set
			{
				airspeedkmh = value;
				if (syncFlag.UpdateUI)
					PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Airspeedkmh"));
			}
		}

		public double Voltage
		{
			get => voltage;
			set
			{
				voltage = value;
				if (syncFlag.UpdateUI)
					PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Voltage"));
			}
		}

		public SyncFlag SyncFlag { get => syncFlag; set => syncFlag = value; }

		public int SSTime
		{
			get => ssTime;
			set
			{
				ssTime = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("SSTime"));
			}
		}

		public bool SSEanbled
		{
			get => ssEanbled;
			set
			{
				ssEanbled = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("SSEanbled"));
			}
		}

		public ObservableCollection<double> GraphData
		{
			get => graphData;
			set
			{
				graphData = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("GraphData"));
			}
		}

		public ObservableCollection<double> GraphMaxData
		{
			get => graphMaxData;
			set
			{
				graphMaxData = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("GraphMaxData"));
			}
		}

		public ObservableCollection<double> TableData 
		{
			get => tableData;
			set
			{
				tableData = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("TableData"));
			}
		}

		public double Timer
		{
			get => timer;
			set
			{
				timer = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Timer"));
			}
		}

		public double Freqency
		{
			get => freqency;
			set
			{
				freqency = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Freqency"));
			}
		}

		public bool SampleRateChanged { get => sampleRateChanged; set => sampleRateChanged = value; }
		public bool VerisonChanged { get => verisonChanged; set => verisonChanged = value; }
		public int CurrentVerison { get => currentVerison; set => currentVerison = value; }
		public int SampleRate
		{
			get => sampleRate;
			set
			{
				if (sampleRate != value)
					sampleRateChanged = true;
				sampleRate = value;
				sampleInterval = 1.0 / sampleRate;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("SampleRate"));
			}
		}

		public ADCRange[] SamplingRange { get => samplingRange; set => samplingRange = value; }
		public double SampleInterval { get => sampleInterval; }

		public FlightState(SyncFlag sf)
		{
			syncFlag = sf;
			SSTime = 2000;
			SSEanbled = false;
			graphData = new ObservableCollection<double>();
			tableData = new ObservableCollection<double>();
			graphMaxData = new ObservableCollection<double>();
			for (int i = 0; i < 10; i++)
			{
				graphData.Add(0);
				tableData.Add(0);
				graphMaxData.Add(0);
			}
			verisonChanged = true;
			sampleRateChanged = true;
			currentVerison = -1;
			sampleRate = 1000;
			sampleInterval = 0.001;
			samplingRange = new ADCRange[2] { ADCRange.Bipolar5V0, ADCRange.Bipolar5V0 };
			
		}
	}
}

public enum ADCRange : byte
{
	Bipolar2V5 = 0,
	Bipolar5V0,
	Bipolar6V25,
	Bipolar10V0,
	Bipolar12V5,
	Bipolar20V0,
	Unipolar5V0,
	Unipolar10V0
}
