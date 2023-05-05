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
		bool updateGraph;
		int skipCount;
		int cnt;
		int graphSkipCnt;
		int graphCnt;

		public bool UpdateUI { get => updateUI; set => updateUI = value; }
		public int SkipCount { get => skipCount; set => skipCount = value; }
		public int GraphSkipCnt { get => graphSkipCnt; set => graphSkipCnt = value; }
		public bool UpdateGraph { get => updateGraph; set => updateGraph = value; }

		public SyncFlag()
		{
			updateUI = true;
			updateGraph = true;
			graphCnt = 0;
			graphSkipCnt = 0;
			cnt = 0;
			skipCount = 0;
		}
		public void UpdateFlag()
		{
			cnt++;
			graphCnt++;
			if(cnt>=skipCount)
			{
				cnt = 0;
				updateUI = true;
			}
			if(graphCnt>graphSkipCnt)
			{
				graphCnt = 0;
				UpdateGraph = true;
			}

		}
		public void ResetFlags()
		{
			updateUI = false;
			updateGraph = false;
		}
	}
}
