using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;
using System.Diagnostics;
using System.IO;
using System.Collections.ObjectModel;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using Microsoft.Research.DynamicDataDisplay.ViewportRestrictions;
using SharpBladeDAS;
using System.Threading;

namespace NickelBird
{
	/// <summary>
	/// MainWindow.xaml 的交互逻辑
	/// </summary>
	public partial class MainWindow : Window, INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		AdvancedPortScanner portScanner;
		SerialLink link;
		Config config;
		System.Threading.Timer linkMonitor;
		bool isLogging;
		bool isWritingFile;
		StreamWriter logWriter;
		ObservableCollection<SensorData> sensorDatas;
		double currentTime;

		FlightState flightState;

		List<ObservableDataSource<Point>> graphData;
		List<AxesRangeRestriction> graphRestrictions;

		int graphcnt;

		int trim;
		int trimcnt;
		double trimsum;
		int logcnt;
		double plotTime;
		int logStartTime;
		SyncFlag syncFlag;

		string sepChar;
		int sscnt;

		ObservableCollection<string> logNameElements;

		List<double> graphSum;
		List<double> graphMax;
		//List<int> graphChannels;
		List<ComboBox> graphNames;

		Queue<int> trimList;

		List<ChartPlotter> plotters;

		SimpleMatrix mat;
		SimpleMatrix logmat;

		List<double> buff;

		Thread dataListenerThread;
		bool linkAvilable;

		List<double> freqbuff;

		int freqHead, freqCount;
		SensorData freqData;
		public ObservableCollection<string> LogNameElements
		{
			get => logNameElements;
			set
			{
				logNameElements = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("LogNameElements"));
			}
		}

		public MainWindow()
		{
			InitializeComponent();
			System.Globalization.CultureInfo myCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
			myCulture.NumberFormat.NaNSymbol = "NaN";
			myCulture.NumberFormat.PositiveInfinitySymbol = "inf";
			myCulture.NumberFormat.NegativeInfinitySymbol = "-inf";
			System.Globalization.CultureInfo.DefaultThreadCurrentCulture = myCulture;
			System.Threading.Thread.CurrentThread.CurrentCulture = myCulture;

			sensorDatas = new ObservableCollection<SensorData>();
			buff = new List<double>();
			syncFlag = new SyncFlag();
			syncFlag.SkipCount = 50;
			syncFlag.GraphSkipCnt = 10;
			initConfig();
			//sensorDatas.Add(new SensorData(syncFlag, config) { Name="Freq"});
			freqData = new SensorData(syncFlag, config);
			freqText.DataContext = freqData;

			logNameElements = new ObservableCollection<string>();
			for (int i = 0; i < config.LogName.Length; i++)
			{
				logNameElements.Add(config.LogName[i]);

			}

			sensorDataList.DataContext = sensorDatas;
			pathText.DataContext = config;
			filterGrid.DataContext = config;
			configGrid.DataContext = config;



			plotters = new List<ChartPlotter>();
			plotters.Add(Plotter1);
			plotters.Add(Plotter2);
			plotters.Add(Plotter3);
			plotters.Add(Plotter4);
			plotters.Add(Plotter5);
			plotters.Add(Plotter6);

			graphData = new List<ObservableDataSource<Point>>();
			graphRestrictions = new List<AxesRangeRestriction>();
			for (int i = 0; i < plotters.Count; i++)
			{
				graphData.Add(new ObservableDataSource<Point>());
				graphRestrictions.Add(new AxesRangeRestriction() { XRange = new DisplayRange(0, 2) });
				plotters[i].AddLineGraph(graphData[i], Colors.Red);
				plotters[i].Viewport.Restrictions.Add(graphRestrictions[i]);
				plotters[i].LegendVisible = false;
			}


			isLogging = false;
			isWritingFile = false;

			flightState = new FlightState(syncFlag);
			flightState.Airspeed = 0;

			graphGrid.DataContext = flightState;
			logNameGrid.DataContext = this;
			configNameGrid.DataContext = this;

			trim = -1;

			graphSum = new List<double>();
			graphMax = new List<double>();
			for (int i = 0; i < plotters.Count; i++)
			{
				graphSum.Add(0);
				graphMax.Add(0);
			}
			graphcnt = 0;
			trimList = new Queue<int>();
			sepChar = "\t";
			sscnt = 0;
			sstextBox.DataContext = flightState;
			csvcheckBox.DataContext = config;
			//graphChannels = new List<int>();
			//for (int i = 0; i < plotters.Count; i++)
			//	graphChannels.Add(0);
			graphNames = new List<ComboBox>();
			graphNames.Add(GraphName1);
			graphNames.Add(GraphName2);
			graphNames.Add(GraphName3);
			graphNames.Add(GraphName4);
			graphNames.Add(GraphName5);
			graphNames.Add(GraphName6);
			for (int i = 0; i < plotters.Count; i++)
			{
				graphNames[i].ItemsSource = sensorDatas;
				graphNames[i].DisplayMemberPath = "FullName";
				//graphNames[i].SelectedIndex = config.PlotData[i];
				graphNames[i].DataContext = config;
			}

			tableName1.ItemsSource = sensorDatas;
			tableName1.DisplayMemberPath = "FullName";
			tableName1.DataContext = config;
			tableName2.ItemsSource = sensorDatas;
			tableName2.DisplayMemberPath = "FullName";
			tableName2.DataContext = config;
			tableDataGrid.DataContext = flightState;

			freqName.ItemsSource = sensorDatas;
			freqName.DisplayMemberPath = "FullName";
			freqName.DataContext = config;
			freqcheckBox.DataContext = config;

			timerText.DataContext = flightState;
			freqText.DataContext = flightState;
			samplingRateText.DataContext = flightState;

			linkAvilable = false;
			dataListenerThread = new Thread(dataListenerWorker);
			dataListenerThread.IsBackground = true;
			dataListenerThread.Priority = ThreadPriority.AboveNormal;
			dataListenerThread.Start();

			freqbuff = new List<double>(1024);
			for (int i = 0; i < 1024; i++)
				freqbuff.Add(0);

			// portScanner = new AdvancedPortScanner(2000000, 256, 3);
			portScanner = new AdvancedPortScanner(config.Baudrate, 256, 3);
			portScanner.OnFindPort += PortScanner_OnFindPort;
			portScanner.Start();
			linkMonitor = new System.Threading.Timer(linkMonitorWorker, null, 0, 1000);
		}



		private void PortScanner_OnFindPort(AdvancedPortScanner sender, PortScannerEventArgs e)
		{
			Debug.WriteLine("[main] find port {0}", e.Link.Port.PortName);
			link = e.Link;
			link.OnReceivePackage += Link_OnReceivePackage;
			link.OpenPort();
			portScanner.Stop();
			linkAvilable = true;
		}

		private void dataListenerWorker()
		{
			while (true)
			{
				//while (linkAvilable && link.ReceivedPackageQueue.TryDequeue(out LinkPackage package))
				while (linkAvilable && link.ReceivedPackageCollection.TryTake(out LinkPackage package, 2))
				{
					analyzePackage((SBLinkPackage)package);
					//Debug.WriteLine("[Link] Recevie package size= {0} function={1} remain={2}", package.DataSize,package.Function,link.ReceivedPackageQueue.Count);
				}
			}
		}

		private void linkMonitorWorker(object state)
		{
			if (link != null)
			{
				if (linkAvilable)
				{
					if (!link.Port.IsOpen)
					{
						linkAvilable = false;
						link = null;
						flightState.SampleRateChanged = true;
						flightState.VerisonChanged = true;
						flightState.CurrentVerison = -1;
						portScanner.Start();
					}
				}
			}
		}

		private void Link_OnReceivePackage(CommLink sender, LinkEventArgs e)
		{

		}

		bool checkPackageVersion(SBLinkPackage package)
		{
			bool res = true;
			flightState.CurrentVerison = package.Function - 1;
			if (flightState.CurrentVerison == 2)
			{
				byte b = package.NextByte();
				ADCRange r1 = (ADCRange)(b & 0x07);
				ADCRange r2 = (ADCRange)((b >> 3) & 0x07);
				if (flightState.SamplingRange[0] != r1 || flightState.SamplingRange[1] != r2)
				{
					flightState.SamplingRange[0] = r1;
					flightState.SamplingRange[1] = r2;
					flightState.VerisonChanged = true;
				}
				res = (b & 0b10000000)!=0;
				package.NextByte();
				flightState.SampleRate = package.NextShort();
				if(flightState.SampleRateChanged)
				{
					syncFlag.SkipCount = flightState.SampleRate / 20;
					syncFlag.GraphSkipCnt = flightState.SampleRate / 100;
				}
				
			}
			if (flightState.VerisonChanged)
			{
				if (flightState.CurrentVerison == 0)
				{
					flightState.SampleRate = 1000;
					for (int i = 0; i < 8; i++)
					{
						sensorDatas[i].Factor = 10.0 / 65536;
					}
					for (int i = 8; i < 10; i++)
					{
						sensorDatas[i].Factor = 3.3 / 4096;
					}
				}
				if (flightState.CurrentVerison == 1)
				{
					flightState.SampleRate = 1000;
					for (int i = 0; i < 8; i++)
					{
						sensorDatas[i].Factor = 10.0 / 65536;
					}
					for (int i = 8; i < 12; i++)
					{
						sensorDatas[i].Factor = 3.3 / 4096;
					}
				}
				if (flightState.CurrentVerison == 2)
				{
					
					for (int i = 0; i < 8; i++)
					{
						sensorDatas[i].Factor = SensorData.RangeFactor[(int)flightState.SamplingRange[0]];
					}
					for (int i = 8; i < 16; i++)
					{
						sensorDatas[i].Factor = SensorData.RangeFactor[(int)flightState.SamplingRange[1]];
					}
				}
				flightState.VerisonChanged = false;
			}
			return res;
		}

		void getPackageData(SBLinkPackage package)
		{			
			if(flightState.CurrentVerison==0)
			{
				//8 x 16bit + 2 x 12bit + 4 x float
				for (int i = 0; i < 8; i++)
				{
					sensorDatas[i].SetRawValue(package.NextShort());
					buff[i]= sensorDatas[i].LpfValue;
				}
				for (int i = 8; i < 10; i++)
				{
					sensorDatas[i].SetRawValue(package.NextShort());
					buff[i] = sensorDatas[i].LpfValue;
				}
				for (int i = 16; i < 20; i++)
				{
					sensorDatas[i].SetRawValue(package.NextSingle());
					buff[i] = sensorDatas[i].LpfValue;
				}
			}
			else if(flightState.CurrentVerison == 1)
			{
				//8 x 16bit + 4 x 12bit + 4 x float
				for (int i = 0; i < 8; i++)
				{
					sensorDatas[i].SetRawValue(package.NextShort());
					buff[i] = sensorDatas[i].LpfValue;
				}
				for (int i = 8; i < 12; i++)
				{
					sensorDatas[i].SetRawValue(package.NextShort());
					buff[i] = sensorDatas[i].LpfValue;
				}
				for (int i = 16; i < 20; i++)
				{
					sensorDatas[i].SetRawValue(package.NextSingle());
					buff[i] = sensorDatas[i].LpfValue;
				}
			}
			else if (flightState.CurrentVerison == 2)
			{
				//16 x 16bit + 4 x float
				for (int i = 0; i < 16; i++)
				{
					sensorDatas[i].SetRawValue(package.NextShort());
					buff[i] = sensorDatas[i].LpfValue;
				}
				for (int i = 16; i < 20; i++)
				{
					sensorDatas[i].SetRawValue(package.NextSingle());
					buff[i] = sensorDatas[i].LpfValue;
				}
			}			
		}

		void transformData()
		{
			mat.Transform(buff, 0, buff, 20);
			logmat.TransformLog(buff, 0, buff, 20);
			for (int i = 20; i < buff.Count; i++)
				sensorDatas[i].SetRawValue((float)buff[i]);
		}

		void analyzePackage(SBLinkPackage package)
		{
			bool stopFlag = false;
			bool logData = true;
			if (isLogging)
				isWritingFile = true;
			plotTime += flightState.SampleInterval;
			
			package.StartRead();
			if (logStartTime < 0)
				logStartTime = package.Time;
			//currentTime = (package.Time - logStartTime) / config.FilterFs;
			currentTime = (double)(package.Time - logStartTime) / flightState.SampleRate;


			logData =checkPackageVersion(package);
			

			if (package.Function == 1 || package.Function == 2 || package.Function == 3)
			{
				int pos = 0;
				syncFlag.UpdateFlag();

				getPackageData(package);
				transformData();

				if (isLogging)
				{
					isWritingFile = true;
					if (logcnt >= config.LogSkip)
					{
						if (logData)
						{
							logWriter.Write(currentTime.ToString());
							for (int i = 0; i < sensorDatas.Count; i++)
							{
								if (sensorDatas[i].LogEnabled)
								{
									logWriter.Write(sepChar + sensorDatas[i].ScaledValue);
									logWriter.Write(sepChar + sensorDatas[i].RawValue);
									if (sensorDatas[i].FilterEnabled)
									{
										logWriter.Write(sepChar + sensorDatas[i].LpfValue);
									}
								}
							}
							if (config.IsCsv)
								logWriter.Write(",");
							logWriter.Write(Environment.NewLine);
							logWriter.Flush();
							flightState.Timer += 1;
						}
						isWritingFile = false;
						
						logcnt = 0;
					}
					isWritingFile = false;
					logcnt++;
					//flightState.Timer += 0.001;
					

					if (flightState.SSEanbled)
					{
						sscnt++;
						if (sscnt >= flightState.SSTime)
						{
							stopFlag = true;
						}
					}
				}
				for (int i = 0; i < plotters.Count; i++)
				{
					if (Math.Abs(sensorDatas[config.PlotData[i]].ScaledValue) > Math.Abs(graphMax[i]))
					{
						graphMax[i] = sensorDatas[config.PlotData[i]].ScaledValue;
					}
				}
				if (syncFlag.UpdateGraph)
				{
					for (int i = 0; i < plotters.Count; i++)
					{
						graphData[i].AppendAsync(Dispatcher, new Point(plotTime, sensorDatas[config.PlotData[i]].LpfValue));
						graphSum[i] += sensorDatas[config.PlotData[i]].LpfValue;

					}
					for (int i = 0; i < 2; i++)
					{
						flightState.TableData[i] = sensorDatas[config.TableData[i]].LpfValue;
					}
					graphcnt++;
					if (plotTime > 2)
					{
						plotTime = 0;
						Dispatcher.Invoke(new Action(() =>
						{
							for (int i = 0; i < plotters.Count; i++)
								graphData[i].Collection.Clear();
						}));
						for (int i = 0; i < plotters.Count; i++)
						{
							flightState.GraphData[i] = graphSum[i] / graphcnt;
							flightState.GraphMaxData[i] = graphMax[i];
							graphSum[i] = 0;
							graphMax[i] = 0;
						}
						graphcnt = 0;
					}
				}
				if (config.FreqEnabled)
				{
					freqbuff[(freqHead + freqCount) % freqbuff.Count] = sensorDatas[config.FreqData].LpfValue;
					freqCount++;
					if (freqCount >= 1000)
					{
						freqHead += freqCount - 1000;
						freqHead %= freqbuff.Count;
						freqCount = 1000;
						if (syncFlag.UpdateUI)
							flightState.Freqency = (float)SharpBladeDAS.MathHelper.CorrFreq(freqbuff, freqHead, freqCount);
					}
				}

				if (stopFlag)
				{
					Dispatcher.Invoke(new Action(() =>
					{
						stopLog();
					}));
				}

				if (trim >= 0)
				{
					trimsum += sensorDatas[trim].Value;
					trimcnt++;
					if (trimcnt >= 1000)
					{
						sensorDatas[trim].Offset = -trimsum / trimcnt;
						if (trimList.Count == 0)
						{
							trim = -1;
						}
						else
						{
							trimsum = 0;
							trimcnt = 0;
							trim = trimList.Dequeue();
						}
					}
				}
				syncFlag.ResetFlags();
			}
		}
		void initConfig()
		{
			string path = Environment.CurrentDirectory;
			DirectoryInfo di = new DirectoryInfo(path + "\\config");
			if (!di.Exists)
				di.Create();
			FileInfo fi = new FileInfo(path + "\\config\\configFile.txt");
			if (!fi.Exists)
			{
				StreamWriter sw = new StreamWriter(path + "\\config\\configFile.txt");
				sw.WriteLine("config.xml");
				sw.Close();
			}
			StreamReader sr = new StreamReader(path + "\\config\\configFile.txt");
			string configName = sr.ReadLine();
			sr.Close();
			XmlSerializer xs = new XmlSerializer(typeof(Config));
			Stream s = new FileStream(path + "\\config\\" + configName, FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.None);
			try
			{
				config = (Config)xs.Deserialize(s);
				config.ConfigName = configName;
				s.Close();
			}
			catch
			{
				s.Close();
				s = new FileStream(path + "\\config\\config.xml", FileMode.Create, FileAccess.ReadWrite, FileShare.None);
				config = Config.Default(23);
				xs.Serialize(s, config);
				s.Close();

			}
			initMatrix();
			sensorDatas.Clear();
			initSensorDataConfig();

		}

		void initSensorDataConfig()
		{
			int dataCnt = mat.Row + 20;// 20 * original data + transformed data

			// Add more data config
			while (config.Offsets.Count < dataCnt)
			{
				config.Offsets.Add(0);
				config.Scales.Add(1);
				config.LogEnabled.Add(false);
				config.Names.Add("Sensor" + config.Offsets.Count.ToString());
				config.FilterEnabled.Add(false);
				config.Factors.Add(1);
			}

			while (sensorDatas.Count > dataCnt)
				sensorDatas.RemoveAt(sensorDatas.Count - 1);

			for (int i = sensorDatas.Count; i < dataCnt; i++)
			{
				sensorDatas.Add(new SensorData(syncFlag, config)
				{
					Id = i,
					Offset = config.Offsets[i],
					Scale = config.Scales[i],
					LogEnabled = config.LogEnabled[i],
					Name = config.Names[i],

					FilterEnabled = config.FilterEnabled[i],
					Factor = config.Factors[i]
				});
				buff.Add(0);
			}

			//check data reference
			for (int i = 0; i < 6; i++)
			{
				if (config.PlotData[i] >= dataCnt)
					config.PlotData[i] = i;
			}
			for (int i = 0; i < 2; i++)
			{
				if (config.TableData[i] >= dataCnt)
					config.TableData[i] = i;
			}
			if (config.FreqData >= dataCnt)
				config.FreqData = 0;
		}

		void initMatrix()
		{
			string path = Environment.CurrentDirectory + "\\matrices";
			DirectoryInfo di = new DirectoryInfo(path);
			if (!di.Exists)
				di.Create();
			string name = config.MatrixName;
			FileInfo fi = new FileInfo(path + "\\" + name);
			
			string str;
			string[] strs;
			int row, col;
			int i = 0;
			if (fi.Exists)
			{
				StreamReader sr = new StreamReader(path + "\\" + name);
				str = sr.ReadLine();
				strs = str.Split(',');
				row = int.Parse(strs[0]);
				col = int.Parse(strs[1]);
				mat = new SimpleMatrix(row, col, false);
				logmat = new SimpleMatrix(row, col, true);
				sr.ReadLine();//skip header				
				for (; i < row ; i++)
				{
					str = sr.ReadLine();
					strs = str.Split(',');
					for (int j = 0; j < col + 1; j++)
					{
						mat.Data[i, j] = double.Parse(strs[j + 1]);//skip header
					}
				}
				for (; i < row; i++)
					sr.ReadLine();
				sr.ReadLine();//skip space line
				sr.ReadLine();//skip header
				for (i = 0; i < row ; i++)
				{
					str = sr.ReadLine();
					strs = str.Split(',');
					for (int j = 0; j < col + 1; j++)
					{
						logmat.Data[i, j] = double.Parse(strs[j + 1]);
					}
				}
				sr.Close();
			}
			else
			{
				showMessage("处理矩阵不存在！");
				config.MatrixName = "NoMatrix.csv";
				StreamWriter sw = new StreamWriter(path + "\\" + config.MatrixName);				
				sw.Write("0,0");		
				//for (i = 1; i <= c + 1; i++)
				//	sw.Write(",");
				//sw.WriteLine();
				////write header
				//sw.Write(",");
				//for (i = 0; i < c; i++)
				//	sw.Write(i.ToString() + ",");
				//sw.Write("Offset,");
				//sw.WriteLine();
				//for (i = 0; i < r; i++)
				//{
				//	sw.Write(i.ToString() + ",");//header
				//	for (int j = 0; j < c + 1; j++)
				//	{
				//		sw.Write(mat.Data[i, j]);
				//		sw.Write(",");
				//	}
				//	sw.WriteLine();

				//}
				////space line
				//for (i = 0; i <= c; i++)
				//{
				//	sw.Write(",");
				//}
				//sw.WriteLine();
				////write header
				//sw.Write(",");
				//for (i = 0; i < c; i++)
				//	sw.Write(i.ToString() + ",");
				//sw.Write("Offset,");
				//sw.WriteLine();
				//for (i = 0; i < r; i++)
				//{
				//	sw.Write(i.ToString() + ",");
				//	for (int j = 0; j < c + 1; j++)
				//	{
				//		sw.Write(logmat.Data[i, j]);
				//		sw.Write(",");
				//	}
				//	sw.WriteLine();
				//}
				sw.Close();
				mat = new SimpleMatrix(0, 0, false);
				logmat = new SimpleMatrix(0, 0, true);
			}

		}



		private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			saveConfig();
			//StreamWriter sw = new StreamWriter(Environment.CurrentDirectory + "\\config\\configFile.txt");
			//sw.WriteLine("config.xml");
			//sw.Close();
		}

		private void saveConfig()
		{
			for (int i = 0; i < sensorDatas.Count; i++)
			{
				config.Names[i] = sensorDatas[i].Name;
				config.Offsets[i] = sensorDatas[i].Offset;
				config.Scales[i] = sensorDatas[i].Scale;
				config.LogEnabled[i] = sensorDatas[i].LogEnabled;
				config.FilterEnabled[i] = sensorDatas[i].FilterEnabled;

			}
			for (int i = 0; i < config.LogName.Length; i++)
			{
				config.LogName[i] = logNameElements[i];

			}
			config.LogNameBackup[0] = nameText1.Text;
			config.LogNameBackup[1] = nameText2.Text;
			config.LogNameBackup[2] = nameText3.Text;
			config.LogNameBackup[3] = nameText4.Text;
			config.LogNameBackup[4] = nameText5.Text;
			config.LogNameBackup[5] = nameText.Text;

			XmlSerializer xs = new XmlSerializer(typeof(Config));
			Stream s = new FileStream(Environment.CurrentDirectory + "\\config\\" + config.ConfigName, FileMode.Create, FileAccess.Write, FileShare.None);
			xs.Serialize(s, config);
			s.Close();
		}

		private void LogBtn_Click(object sender, RoutedEventArgs e)
		{
			if (isLogging)
			{
				stopLog();
			}
			else
			{
				startLog();
			}
		}

		private void startLog()
		{
			saveConfig();
			logBtn.Content = "停止采集";
			sensorDataList.IsEnabled = false;
			configTab.IsEnabled = false;
			ssbutton.IsEnabled = false;
			if (flightState.SSEanbled)
			{
				logBtn.IsEnabled = false;
			}
			string name = "";
			if (logNameElements[0] != "")
				name += "_" + ((nameText1.Text != "") ? nameText1.Text : " ");
			if (logNameElements[1] != "")
				name += "_" + ((nameText2.Text != "") ? nameText2.Text : " ");
			if (logNameElements[2] != "")
				name += "_" + ((nameText3.Text != "") ? nameText3.Text : " ");
			if (logNameElements[3] != "")
				name += "_" + ((nameText4.Text != "") ? nameText4.Text : " ");
			if (logNameElements[4] != "")
				name += "_" + ((nameText5.Text != "") ? nameText5.Text : " ");
			createFolder(config.LogPath);
			if (csvcheckBox.IsChecked == true)
				logWriter = new StreamWriter(config.LogPath + "\\" + DateTime.Now.ToString("yyyy-MM-dd-HH-mm-ss") + name + "_" + nameText.Text + ".csv");
			else
				logWriter = new StreamWriter(config.LogPath + "\\" + DateTime.Now.ToString("yyyy-MM-dd-HH-mm-ss") + name + "_" + nameText.Text + ".txt");
			logWriter.Write("Time");
			for (int i = 0; i < sensorDatas.Count; i++)
			{
				if (sensorDatas[i].LogEnabled)
				{
					logWriter.Write(sepChar + sensorDatas[i].Name);
					logWriter.Write(sepChar + sensorDatas[i].Name + "_RAW");
					if (sensorDatas[i].FilterEnabled)
						logWriter.Write(sepChar + sensorDatas[i].Name + "_LPF");
				}
			}
			if (csvcheckBox.IsChecked == true)
				logWriter.Write(",");
			logWriter.Write(Environment.NewLine);
			logWriter.Flush();
			logStartTime = -1;
			currentTime = 0;
			sscnt = 0;
			logcnt = 1;
			isLogging = true;
		}

		private void stopLog()
		{
			isLogging = false;
			//Wait for all writing complete								
			while (isWritingFile) ;
			logWriter.Close();
			logBtn.Content = "开始采集";

			sensorDataList.IsEnabled = true;
			configTab.IsEnabled = true;
			ssbutton.IsEnabled = true;
			if (flightState.SSEanbled)
			{
				logBtn.IsEnabled = true;
			}
			flightState.SSEanbled = false;
			//if (flightState.SSEanbled)
			//{
			//	logBtn.IsDefault = true;
			//	ssbutton.IsDefault = false;
			//}
			// auto inc name
			int orignalVal = 0;
			int incVal = 0;

			if (int.TryParse(nameText1.Text, out orignalVal) && int.TryParse(nameIncText1.Text, out incVal))
			{
				nameText1.Text = (orignalVal + incVal).ToString();
			}
			if (int.TryParse(nameText2.Text, out orignalVal) && int.TryParse(nameIncText2.Text, out incVal))
			{
				nameText2.Text = (orignalVal + incVal).ToString();
			}
			if (int.TryParse(nameText3.Text, out orignalVal) && int.TryParse(nameIncText3.Text, out incVal))
			{
				nameText3.Text = (orignalVal + incVal).ToString();
			}
			if (int.TryParse(nameText4.Text, out orignalVal) && int.TryParse(nameIncText4.Text, out incVal))
			{
				nameText4.Text = (orignalVal + incVal).ToString();
			}
			if (int.TryParse(nameText5.Text, out orignalVal) && int.TryParse(nameIncText5.Text, out incVal))
			{
				nameText5.Text = (orignalVal + incVal).ToString();
			}
			flightState.Timer = 0;
		}



		private void SaveLogNameBtn_Click(object sender, RoutedEventArgs e)
		{
			string name = "";
			for (int i = 0; i < logNameElements.Count; i++)
			{
				if (logNameElements[i] != "")
				{
					name += "_";
					name += logNameElements[i];
				}
			}
			createFolder(config.LogPath);
			StreamWriter sw = new StreamWriter(config.LogPath + "\\" + DateTime.Now.ToString("yyyy-MM-dd-HH-mm-ss") + name + ".txt");
			sw.WriteLine("记录文件命名方式");
			sw.Write("时间" + name);
			sw.Close();
		}

		private void PathBtn_Click(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.FolderBrowserDialog fbdig = new System.Windows.Forms.FolderBrowserDialog();
			var dr = fbdig.ShowDialog();
			if (dr != System.Windows.Forms.DialogResult.Cancel)
			{
				config.LogPath = fbdig.SelectedPath;
			}
		}


		private void Window_Loaded(object sender, RoutedEventArgs e)
		{
			nameText1.Text = config.LogNameBackup[0];
			nameText2.Text = config.LogNameBackup[1];
			nameText3.Text = config.LogNameBackup[2];
			nameText4.Text = config.LogNameBackup[3];
			nameText5.Text = config.LogNameBackup[4];
			nameText.Text = config.LogNameBackup[5];
		}

		private void CsvcheckBox_Checked(object sender, RoutedEventArgs e)
		{
			if (csvcheckBox.IsChecked == true)
				sepChar = ",";
			else
				sepChar = "\t";
		}

		private void Ssbutton_Click(object sender, RoutedEventArgs e)
		{
			if (!isLogging)
			{
				flightState.SSEanbled = true;
				startLog();
			}
		}

		private void TrimBtn_Click(object sender, RoutedEventArgs e)
		{
			if (trim < 0)
			{
				trimsum = 0;
				trimcnt = 0;
				trim = sensorDataList.SelectedIndex;
			}

		}

		private void RstScopeBtn_Click(object sender, RoutedEventArgs e)
		{
			for (int i = 0; i < 6; i++)
				graphRestrictions[i].Reset();
		}

		private void createFolder(string path)
		{
			string[] pathes = path.Split('\\');
			if (pathes.Length > 1)
			{
				string str = pathes[0];
				for (int i = 1; i < pathes.Length; i++)
				{
					str += "\\" + pathes[i];
					if (!Directory.Exists(str))
						Directory.CreateDirectory(str);
				}
			}
		}

		private void showMessage(string msg)
		{
			MessageBox.Show(msg, "NickelBird");
		}

		private void openConfigBtn_Click(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.OpenFileDialog ofd = new System.Windows.Forms.OpenFileDialog();
			ofd.InitialDirectory = Environment.CurrentDirectory + "\\config";
			if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string path = ofd.FileName;
				FileInfo fi = new FileInfo(path);
				string name = fi.Name;
				if (fi.DirectoryName != (Environment.CurrentDirectory + "\\config"))
				{
					string newpath = Environment.CurrentDirectory + "\\config\\" + name;
					FileInfo nfi = new FileInfo(newpath);
					if (nfi.Exists)
					{
						if (MessageBox.Show("存在同名配置文件，是否覆盖？", "NickelBird", MessageBoxButton.YesNo) == MessageBoxResult.No)
							return;
					}
					fi.CopyTo(nfi.FullName, true);
				}
				StreamWriter sw = new StreamWriter(Environment.CurrentDirectory + "\\config\\configFile.txt");
				sw.WriteLine(name);
				sw.Close();
				//initConfig();
				showMessage("重启应用程序启用新配置文件");
			}
		}

		private void openMatrixBtn_Click(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.OpenFileDialog ofd = new System.Windows.Forms.OpenFileDialog();
			ofd.InitialDirectory = Environment.CurrentDirectory + "\\matrices";
			if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string path = ofd.FileName;
				FileInfo fi = new FileInfo(path);
				string name = fi.Name;
				if (fi.DirectoryName != (Environment.CurrentDirectory + "\\matrices"))
				{
					string newpath = Environment.CurrentDirectory + "\\matrices\\" + name;
					FileInfo nfi = new FileInfo(newpath);
					if (nfi.Exists)
					{
						if (MessageBox.Show("存在同名配置文件，是否覆盖？", "NickelBird", MessageBoxButton.YesNo) == MessageBoxResult.No)
							return;
					}
					fi.CopyTo(nfi.FullName, true);
				}
				config.MatrixName = name;
				initMatrix();
				initSensorDataConfig();
				//initConfig();
				//showMessage("重启应用程序启用新配置文件");
			}
		}

		private void GraphTrim1_Click(object sender, RoutedEventArgs e)
		{
			if (trim < 0)
			{
				trimsum = 0;
				trimcnt = 0;
				trim = graphNames[0].SelectedIndex;
			}
		}

		private void GraphTrim2_Click(object sender, RoutedEventArgs e)
		{
			if (trim < 0)
			{
				trimsum = 0;
				trimcnt = 0;
				trim = graphNames[1].SelectedIndex;
			}
		}

		private void GraphTrim3_Click(object sender, RoutedEventArgs e)
		{
			if (trim < 0)
			{
				trimsum = 0;
				trimcnt = 0;
				trim = graphNames[2].SelectedIndex;
			}
		}

		private void GraphTrim4_Click(object sender, RoutedEventArgs e)
		{
			if (trim < 0)
			{
				trimsum = 0;
				trimcnt = 0;
				trim = graphNames[3].SelectedIndex;
			}
		}

		private void GraphTrim5_Click(object sender, RoutedEventArgs e)
		{
			if (trim < 0)
			{
				trimsum = 0;
				trimcnt = 0;
				trim = graphNames[4].SelectedIndex;
			}
		}

		private void GraphTrim6_Click(object sender, RoutedEventArgs e)
		{
			if (trim < 0)
			{
				trimsum = 0;
				trimcnt = 0;
				trim = graphNames[5].SelectedIndex;
			}
		}

		private void importMatrixBtn_Click(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.OpenFileDialog ofd = new System.Windows.Forms.OpenFileDialog();
			ofd.Filter = "ATI校准文件 (*.cal)|*.cal";
			ofd.InitialDirectory = Environment.CurrentDirectory + "\\matrices";
			string[] lines = null;
			if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string path = ofd.FileName;
				lines = readCalFile(path);
				if (lines == null)
				{
					MessageBox.Show("校准文件解析失败", "NickelBird");
					return;
				}

				//initConfig();
				//showMessage("重启应用程序启用新配置文件");
			}
			MessageBox.Show("校准文件解析成功", "NickelBird");
			System.Windows.Forms.SaveFileDialog sfd = new System.Windows.Forms.SaveFileDialog();
			sfd.Filter = "csv文件 (*.csv)|*.csv";
			sfd.AddExtension = true;
			sfd.FileName = System.IO.Path.GetFileNameWithoutExtension(ofd.FileName);
			sfd.InitialDirectory = Environment.CurrentDirectory + "\\matrices";
			if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{

				StreamWriter sw = new StreamWriter(sfd.FileName);
				sw.WriteLine("7,14,,,,,,,,,,,,,,");
				sw.WriteLine(",G1,G2,G3,G4,G5,G6,Ch7,Ch8,Volt,Curr,10,11,12,13,Offset");
				sw.WriteLine("Fx," + lines[0] + "0,0,0,0,0,0,0,0,0");
				sw.WriteLine("Fy," + lines[1] + "0,0,0,0,0,0,0,0,0");
				sw.WriteLine("Fz," + lines[2] + "0,0,0,0,0,0,0,0,0");
				sw.WriteLine("Mx," + lines[3] + "0,0,0,0,0,0,0,0,0");
				sw.WriteLine("My," + lines[4] + "0,0,0,0,0,0,0,0,0");
				sw.WriteLine("Mz," + lines[5] + "0,0,0,0,0,0,0,0,0");
				sw.WriteLine("Power,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1");
				sw.WriteLine(",,,,,,,,,,,,,,,");
				sw.WriteLine(",G1,G2,G3,G4,G5,G6,Ch7,Ch8,Volt,Curr,10,11,12,13,Offset");
				sw.WriteLine("Fx,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101.9716");
				sw.WriteLine("Fy,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101.9716");
				sw.WriteLine("Fz,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101.9716");
				sw.WriteLine("Mx,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1");
				sw.WriteLine("My,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1");
				sw.WriteLine("Mz,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1");
				sw.WriteLine("Power,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1");
				sw.Close();
				config.MatrixName = System.IO.Path.GetFileName(sfd.FileName);
				initMatrix();
				initSensorDataConfig();
			}
		}

		private string[] readCalFile(string path)
		{
			XmlDocument xd = new XmlDocument();
			xd.Load(path);
			XmlNode currNode = xd;
			XmlNodeList list = xd.SelectNodes("//FTSensor/Calibration/UserAxis");

			if (list.Count < 6)
				return null;

			string[] res = new string[6];
			int axisflags = 0;

			foreach (XmlNode n in list)
			{
				if (n.NodeType == XmlNodeType.Element)
				{
					XmlElement xe = (XmlElement)n;
					int currch = -1;
					if (xe.HasAttribute("Name"))
					{
						switch (xe.GetAttribute("Name"))
						{
							case "Fx":
								currch = 0;
								break;
							case "Fy":
								currch = 1;
								break;
							case "Fz":
								currch = 2;
								break;
							case "Tx":
								currch = 3;
								break;
							case "Ty":
								currch = 4;
								break;
							case "Tz":
								currch = 5;
								break;
						}
					}
					if (currch >= 0 && xe.HasAttribute("values"))
					{
						string raw = xe.GetAttribute("values");
						string[] parts = raw.Split(' ');
						List<string> csvLineElements = new List<string>();
						foreach (string s in parts)
						{
							if (s.Length > 1)
							{
								if (double.TryParse(s, out _))
								{
									csvLineElements.Add(s);
								}
							}
						}
						if (csvLineElements.Count == 6)
						{
							res[currch] = "";
							foreach (string s in csvLineElements)
							{
								res[currch] += s;
								res[currch] += ",";
							}
							axisflags |= 1 << currch;
						}
					}
				}
			}

			if (axisflags == 0x3F)
				return res;
			return null;

		}

		private void saveConfigBtn_Click(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.SaveFileDialog sfd = new System.Windows.Forms.SaveFileDialog();
			sfd.Filter = "xml文件 (*.xml)|*.xml";
			sfd.InitialDirectory = Environment.CurrentDirectory + "\\config";
			sfd.AddExtension = true;
			if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				saveConfig();
				FileInfo fi = new FileInfo(Environment.CurrentDirectory + "\\config\\" + config.ConfigName);
				fi.CopyTo(sfd.FileName);
			}
		}

		private void tableName1_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{

		}
	}
}
