using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.ViewportRestrictions;
using System.Windows;

namespace NickelBird
{
	public class AxesRangeRestriction:ViewportRestrictionBase
	{
		public DisplayRange XRange { get; set; }
		public DisplayRange YRange { get; set; }
		public override Rect Apply(Rect oldDataRect, Rect newDataRect, Viewport2D viewport)
		{
			if(YRange==null)
			{
				YRange = new DisplayRange(newDataRect.Top, newDataRect.Bottom);
				YRange.Margin = 0.1;
				return newDataRect;
			}
			newDataRect.X = XRange.Start;
			newDataRect.Width = XRange.End-XRange.Start;
			return YRange.UpdateRange(newDataRect);			
		}

		public void Reset()
		{
			YRange = null;
		}
	}
	public class DisplayRange
	{
		public double Start { get; set; }
		public double End { get; set; }

		public double Margin { get; set; }

		public DisplayRange(double start, double end)
		{
			Start = start;
			End = end;
		}

		public bool IsInRange(Rect r)
		{
			if (r.Top > Start && r.Bottom < End)
				return true;
			return false;
		}

		public Rect UpdateRange(Rect r)
		{			
			if (r.Top < Start)
				Start = r.Top;
			if (r.Bottom > End)
				End = r.Bottom;
			double len = End - Start;
			return new Rect(r.X, Start - len * Margin, r.Width, len * (1 + 2 * Margin));
		}		
	}
}
