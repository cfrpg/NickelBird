using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharpBladeDAS
{
	public class SimpleLowPassFilter : Filter
	{
		double factor;
		double buff;
		public SimpleLowPassFilter()
		{
			factor = 0;
			buff = 0;
		}

		public double Factor
		{
			get => factor;
			set
			{
				factor = value;
				if (factor < 0)
					factor = 0;
				if (factor > 1)
					factor = 1;
				OnPropertyChanged("Factor");
			}
		}

		public override double Process(double value)
		{
			buff = buff * factor + value * (1 - factor);
			return buff;
		}
	}

}
