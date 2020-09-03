using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharpBladeDAS
{
	public class SyncFlag
	{
		bool updateUI;
		int skipCount;
		int cnt;

		public bool UpdateUI { get => updateUI; set => updateUI = value; }
		public int SkipCount { get => skipCount; set => skipCount = value; }

		public SyncFlag()
		{
			updateUI = true;
			cnt = 0;
			skipCount = 0;
		}
		public void UpdateFlag()
		{
			cnt++;
			if(cnt>=skipCount)
			{
				cnt = 0;
				updateUI = true;
			}

		}
		public void ResetFlags()
		{
			updateUI = false;
		}
	}
}
