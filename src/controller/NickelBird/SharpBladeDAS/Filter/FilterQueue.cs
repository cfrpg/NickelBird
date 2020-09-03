using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharpBladeDAS
{
	public class FilterQueue
	{
		int size;
		int count;

		double[] queue;
		int head;
		int tail;
		public int Size { get => size; set => size = value; }
		public int Count { get => count; }

		public FilterQueue(int n)
		{
			size = n;
			count = 0;
			queue = new double[n + 1];
			head = 0;
			tail = 0;
			count = 0;
		}

		public void Enqueue(double v)
		{
			if ((tail + 1) % size == head)
			{
				throw new Exception("Queue full");
			}
			queue[tail] = v;
			tail = (tail + 1) % size;
			count++;
		}

		public double Dequeue()
		{
			if (tail == head)
			{
				//throw new Exception("Empty queue");
				return 0;
			}
			double r = queue[head];
			head = (head + 1) % size;
			count--;
			return r;
		}

		public void Clear()
		{
			head = 0;
			tail = 0;
			count = 0;
		}

		public double this[int index]
		{
			get
			{
				if (index >= count)
					throw new Exception("Index out of range");
				return queue[(head + index) % size];
			}
		}
	}
}
