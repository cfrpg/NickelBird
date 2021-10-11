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
		}
	}
}
