using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SharpBladeDAS
{
	public class SBLinkPackage : LinkPackage
	{
		int time;

		public int Time { get => time; set => time = value; }

		public override int HeaderSize { get => 8; }

		public override LinkProtocol Protocol { get => LinkProtocol.SBLink; }

		public override int PackageSize { get => dataSize + HeaderSize + 1; }

		public SBLinkPackage() : base(280)
		{

		}

		public override PackageParseResult ReadFromBuffer(byte[] buff, int length, int offset)
		{
			return checkBuffer(buff, length, offset);
		}

		public override PackageParseResult ReadFromBuffer(byte[] buff, int length, int offset, out int dataUsed)
		{
			var r = checkBuffer(buff, length, offset);
			dataUsed = DataSize + HeaderSize + 1;
			return r;
		}

		private PackageParseResult checkBuffer(byte[] buff, int length, int offset)
		{
			if (length - offset < HeaderSize)
				return PackageParseResult.NoEnoughData;
			if (!(buff[offset + 0] == 0xFC))
				return PackageParseResult.NoSTX;
			int len = buff[offset + 1];
			if (len + HeaderSize + 1 + offset > length)
				return PackageParseResult.NoEnoughData;
			byte sum = 0;
			for (int i = 0; i < len + HeaderSize; i++)
			{
				sum ^= buff[offset + i];
			}
			if (buff[offset + len + HeaderSize] != sum)
				return PackageParseResult.BadCheckSum;
			for (int i = 0; i < buffer.Length; i++)
			{
				buffer[i] = 0;
			}
			for (int i = 0; i < len + HeaderSize + 1; i++)
			{
				buffer[i] = buff[offset + i];
			}
			function = buffer[3];
			dataSize = len;

			time = BitConverter.ToInt32(buffer, 4);
			//int ms, s, m, h;
			//ms = times & 1023;
			//times >>= 10;
			//s = times & 63;
			//times >>= 6;
			//m = times & 63;
			//times >>= 6;
			//h = times & 31;
			//times >>= 6;

			//time = new DateTime(2019, 5, 1, h, m, s, ms);

			return PackageParseResult.Yes;

		}

		public override bool StartRead()
		{
			cursor = 0;
			return true;
		}

		public override void SetVerify()
		{
			buffer[0] = 0xFC;
			buffer[1] = (byte)dataSize;
			buffer[2] = 0;
			buffer[3] = (byte)function;
			buffer[4] = 0;
			buffer[5] = 0;
			buffer[6] = 0;
			buffer[7] = 0;
			byte sum = 0;
			for (int i = 0; i < dataSize + HeaderSize; i++)
			{
				sum ^= buffer[i];
			}
			AddData(sum);
		}

		public override LinkPackage Clone()
		{
			SBLinkPackage package = new SBLinkPackage();
			buffer.CopyTo(package.buffer, 0);
			package.dataSize = dataSize;
			package.function = function;
			package.time = time;
			package.timeStamp = timeStamp;
			return package;
		}
	}
}
