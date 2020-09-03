using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
		int graphSkipCnt;
		int logcnt;
		double plotTime;
		int logStartTime;
		SyncFlag syncFlag;

		string sepChar;
		int sscnt;
		int sstarget;

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

			sensorDatas = new ObservableCollection<SensorData>();
			buff = new List<double>();
			syncFlag = new SyncFlag();
			syncFlag.SkipCount = 50;
			initConfig();
			

			
			
			logNameElements = new ObservableCollection<string>();
			for (int i = 0; i < config.LogName.Length; i++)
			{
				logNameElements.Add(config.LogName[i]);

			}

			sensorDataList.DataContext = sensorDatas;
			pathText.DataContext = config;
			filterGrid.DataContext = config;			
			configGrid.DataContext = config;

			portScanner = new AdvancedPortScanner(921600, 256, 3);
			portScanner.OnFindPort += PortScanner_OnFindPort;
			portScanner.Start();

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
			sstextBlock.DataContext = flightState;
			sstextBox.DataContext = flightState;
			csvcheckBox.DataContext = config;
			sscheckBox.DataContext = flightState;
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

			timerText.DataContext = flightState;
		}



		private void PortScanner_OnFindPort(AdvancedPortScanner sender, PortScannerEventArgs e)
		{
			Debug.WriteLine("[main] find port {0}", e.Link.Port.PortName);
			link = e.Link;
			link.OnReceivePackage += Link_OnReceivePackage;
			link.OpenPort();
			portScanner.Stop();
		}

		private void Link_OnReceivePackage(CommLink sender, LinkEventArgs e)
		{
			while (link.ReceivedPackageQueue.TryDequeue(out LinkPackage package))
			{
				analyzePackage((SBLinkPackage)package);
				//Debug.WriteLine("[Link] Recevie package size= {0} function={1} remain={2}", package.DataSize,package.Function,link.ReceivedPackageQueue.Count);
			}
		}
		void analyzePackage(SBLinkPackage package)
		{
			plotTime += 0.001;
			graphSkipCnt++;
			package.StartRead();
			if (logStartTime < 0)
				logStartTime = package.Time;
			currentTime = (package.Time - logStartTime) * 0.001;
			switch (package.Function)
			{
				case 1:
					syncFlag.UpdateFlag();
					for (int i = 0; i < 10; i++)
					{
						sensorDatas[i].SetRawValue(package.NextShort());
						buff[i] = sensorDatas[i].LpfValue;
					}
					for (int i = 0; i < 4; i++)
					{
						sensorDatas[i + 10].SetRawValue(package.NextSingle());
						buff[i + 10] = sensorDatas[i + 10].LpfValue;
					}
					for (int i = 14; i < buff.Count; i++)
						buff[i] = 0;
					mat.Transform(buff, 0, buff, 14);
					logmat.TransformLog(buff, 0, buff, 14);
					for (int i = 14; i < buff.Count; i++)
						sensorDatas[i].SetRawValue((float)buff[i]);
					if (isLogging)
					{
						isWritingFile = true;
						if (((!flightState.SSEanbled) || (flightState.SSEanbled && sscnt > 0)) && logcnt >= config.LogSkip)
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
							isWritingFile = false;
							if (flightState.SSEanbled)
							{
								sscnt--;
								if (sscnt <= 0)
								{
									sscnt = 0;
									flightState.SSCount++;
								}
							}
							logcnt = 0;
						}
						logcnt++;
						flightState.Timer += 0.001;
					}
					for (int i = 0; i < plotters.Count; i++)
					{
						if (Math.Abs(sensorDatas[config.PlotData[i]].ScaledValue) > Math.Abs(graphMax[i]))
						{
							graphMax[i] = sensorDatas[config.PlotData[i]].ScaledValue;
						}
					}
					if (graphSkipCnt >= 10)
					{
						for (int i = 0; i < plotters.Count; i++)
						{							
							graphData[i].AppendAsync(Dispatcher, new Point(plotTime, sensorDatas[config.PlotData[i]].LpfValue));
							graphSum[i] += sensorDatas[config.PlotData[i]].LpfValue;
							
						}						
						for(int i=0;i<1;i++)
						{
							flightState.TableData[i] = sensorDatas[config.TableData[i]].LpfValue;
						}
						graphcnt++;
						if (plotTime > 2)
						{
							plotTime = 0;
							Dispatcher.Invoke(new Action(() => {
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

						graphSkipCnt = 0;
					}
					
					if (trim >= 0)
					{
						trimsum += sensorDatas[trim].Value;
						trimcnt++;
						if (trimcnt >= 100)
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
					syncFlag.UpdateUI = false;
					break;
				default:

					break;
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
				config = Config.Default(21);
				xs.Serialize(s, config);
				s.Close();

			}
			initMatrix(7, 14);
			sensorDatas.Clear();
			
			for (int i = 0; i < config.Offsets.Length; i++)
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
			
		}

		void initMatrix(int r, int c)
		{
			string path = Environment.CurrentDirectory + "\\matrices";
			DirectoryInfo di = new DirectoryInfo(path);
			if (!di.Exists)
				di.Create();
			string name = config.MatrixName;
			FileInfo fi = new FileInfo(path + "\\" + name);
			mat = new SimpleMatrix(r, c, false);
			logmat = new SimpleMatrix(r, c, true);
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
				sr.ReadLine();//skip header
				for (; i < row && i < r; i++)
				{
					str = sr.ReadLine();
					strs = str.Split(',');
					for (int j = 0; j < col+1 && j < c+1; j++)
					{
						mat.Data[i, j] = double.Parse(strs[j+1]);//skip header
					}
				}
				for (; i < row; i++)
					sr.ReadLine();
				sr.ReadLine();//skip space line
				sr.ReadLine();//skip header
				for (i=0;i<row&&i<r;i++)
				{
					str = sr.ReadLine();
					strs = str.Split(',');
					for (int j = 0; j < col+1 && j < c+1; j++)
					{
						logmat.Data[i, j] = double.Parse(strs[j+1]);
					}
				}
				sr.Close();
			}
			else
			{
				showMessage("处理矩阵不存在！");
				config.MatrixName = "default"+r.ToString()+"x"+c.ToString()+".csv";
				StreamWriter sw = new StreamWriter(path + "\\" + config.MatrixName);
				sw.Write(r.ToString());
				sw.Write(",");
				sw.Write(c.ToString());
				for (i = 1; i <= c+1; i++)
					sw.Write(",");
				sw.WriteLine();
				//write header
				sw.Write(",");
				for (i = 0; i < c; i++)
					sw.Write(i.ToString()+",");
				sw.Write("Offset,");
				sw.WriteLine();
				for (i = 0; i < r; i++)
				{
					sw.Write(i.ToString()+",");//header
					for(int j=0;j<c+1;j++)
					{
						sw.Write(mat.Data[i, j]);
						sw.Write(",");
					}
					sw.WriteLine();

				}
				//space line
				for(i=0;i<=c;i++)
				{
					sw.Write(",");
				}
				sw.WriteLine();
				//write header
				sw.Write(",");
				for (i = 0; i < c; i++)
					sw.Write(i.ToString() + ",");
				sw.Write("Offset,");
				sw.WriteLine();
				for (i = 0; i < r; i++)
				{
					sw.Write(i.ToString() + ",");
					for (int j = 0; j < c+1; j++)
					{
						sw.Write(logmat.Data[i, j]);
						sw.Write(",");
					}
					sw.WriteLine();
				}
				sw.Close();
			}

		}



		private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			for (int i = 0; i < config.Offsets.Length; i++)
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
			Stream s = new FileStream(Environment.CurrentDirectory + "\\config\\"+config.ConfigName, FileMode.Create, FileAccess.Write, FileShare.None);
			xs.Serialize(s, config);
			s.Close();
			//StreamWriter sw = new StreamWriter(Environment.CurrentDirectory + "\\config\\configFile.txt");
			//sw.WriteLine("config.xml");
			//sw.Close();
		}

		private void LogBtn_Click(object sender, RoutedEventArgs e)
		{
			if (isLogging)
			{
				isLogging = false;
				//Wait for all writing complete
				while (isWritingFile) ;
				logWriter.Close();
				logBtn.Content = "开始采集";

				sensorDataList.IsEnabled = true;
				configTab.IsEnabled = true;
				if (flightState.SSEanbled)
				{
					logBtn.IsDefault = true;
					ssbutton.IsDefault = false;
				}
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
			else
			{
				logBtn.Content = "停止采集";
				sensorDataList.IsEnabled = false;
				configTab.IsEnabled = false;
				if (sscheckBox.IsChecked == true)
				{
					logBtn.IsDefault = false;
					ssbutton.IsDefault = true;
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
				flightState.SSCount = 0;
				logcnt = 1;
				isLogging = true;

			}
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
			if (isLogging && sscnt <= 0)
				sscnt = flightState.SSTime;
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
			ofd.InitialDirectory = Environment.CurrentDirectory+"\\config";
			if(ofd.ShowDialog()== System.Windows.Forms.DialogResult.OK)
			{
				string path = ofd.FileName;
				FileInfo fi = new FileInfo(path);
				string name = fi.Name;
				if(fi.DirectoryName!= (Environment.CurrentDirectory + "\\config"))
				{
					string newpath = Environment.CurrentDirectory + "\\config\\" + name;
					FileInfo nfi = new FileInfo(newpath);
					if(nfi.Exists)
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
				initMatrix(7, 14);
				
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

		private void tableName1_SelectionChanged(object sender, SelectionChangedEventArgs e)
		{

		}
	}
}
