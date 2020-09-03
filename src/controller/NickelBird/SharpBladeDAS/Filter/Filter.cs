using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace SharpBladeDAS
{
	public class Filter : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;
		public Filter()
		{

		}

		public virtual double Process(double value)
		{
			return value;
		}

		protected void OnPropertyChanged(string name)
		{
			PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
		}
	}
}
