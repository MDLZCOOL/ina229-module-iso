using System.IO.Ports;
using FluentModbus;
using Ymodem.Protocol;

namespace 电压电流模块上位机;

public partial class Form1 : Form
{
    private FluentModbus.ModbusRtuClient? _modbusClient;
    private bool _isConnected = false;
    private System.Windows.Forms.Timer _pollTimer = new System.Windows.Forms.Timer();

    public Form1()
    {
        InitializeComponent();

        this.FormBorderStyle = FormBorderStyle.FixedSingle;
        this.MaximizeBox = false;

        _pollTimer.Tick += async (s, e) => await ReadRealtimeDataAsync();
    }

    private void Form1_Load(object sender, EventArgs e)
    {
        ScanSerialPorts();
        FillDefaultBaudrates();
        InitSlaveIdControl();
        InitPollIntervalControl();
    }

    private void ScanSerialPorts()
    {
        cmbPorts.Items.Clear();
        string[] ports = SerialPort.GetPortNames();
        if (ports.Length > 0)
        {
            cmbPorts.Items.AddRange(ports);
            cmbPorts.SelectedIndex = 0;
        }
    }

    private void FillDefaultBaudrates()
    {
        cmbBaud.Items.Clear();
        string[] bauds = ["4800", "9600", "19200", "38400", "57600", "115200"];
        if (bauds.Length > 0)
        {
            cmbBaud.Items.AddRange(bauds);
            cmbBaud.SelectedIndex = 5; // 默认 115200
        }
    }

    private void InitSlaveIdControl()
    {
        if (numSlaveId != null)
        {
            numSlaveId.Minimum = 1;
            numSlaveId.Maximum = 247;
            numSlaveId.Value = 1;
        }
    }

    private void InitPollIntervalControl()
    {
        if (txtPollInterval != null && string.IsNullOrEmpty(txtPollInterval.Text))
        {
            txtPollInterval.Text = "500";
        }
    }

    private int GetPollInterval()
    {
        if (txtPollInterval != null)
        {
            if (int.TryParse(txtPollInterval.Text, out int interval))
            {
                if (interval < 50) return 50; //最小 50ms
                return interval;
            }

            txtPollInterval.Text = "500";
        }

        return 500;
    }

    private void LinkSerialPorts()
    {
        if (_isConnected)
        {
            CloseSerialPort();
            return;
        }

        if (cmbPorts.SelectedItem == null || cmbBaud.SelectedItem == null)
        {
            MessageBox.Show("请先选择串口和波特率！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        string portName = cmbPorts.SelectedItem.ToString()!;
        int baudRate = int.Parse(cmbBaud.SelectedItem.ToString()!);

        try
        {
            _modbusClient = new ModbusRtuClient();

            _modbusClient.BaudRate = baudRate;
            _modbusClient.Parity = Parity.None;
            _modbusClient.StopBits = StopBits.One;
            _modbusClient.ReadTimeout = 1000;
            _modbusClient.WriteTimeout = 1000;

            _modbusClient.Connect(portName, ModbusEndianness.BigEndian);

            _isConnected = true;

            if (btnConnect != null) btnConnect.Text = "断开";
            cmbPorts.Enabled = false;
            cmbBaud.Enabled = false;
            numSlaveId.Enabled = false;
            txtPollInterval.Enabled = false;

            _pollTimer.Interval = GetPollInterval();
            _pollTimer.Start();
        }
        catch (Exception ex)
        {
            MessageBox.Show($"打开串口失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }

    private void CloseSerialPort()
    {
        try
        {
            _pollTimer.Stop();

            if (_modbusClient != null)
            {
                if (_modbusClient.IsConnected)
                {
                    _modbusClient.Close();
                }

                _modbusClient = null;
            }

            _isConnected = false;

            if (btnConnect != null) btnConnect.Text = "连接";
            cmbPorts.Enabled = true;
            cmbBaud.Enabled = true;
            numSlaveId.Enabled = true;
            txtPollInterval.Enabled = true;
        }
        catch (Exception ex)
        {
            MessageBox.Show($"关闭串口异常: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }

    private async Task ReadRealtimeDataAsync()
    {
        if (_modbusClient == null || !_modbusClient.IsConnected)
        {
            return;
        }

        try
        {
            byte slaveId = (byte)numSlaveId.Value;

            Memory<float> result0 = await _modbusClient.ReadHoldingRegistersAsync<float>(slaveId, 0x0000, 6);

            float voltage = result0.Span[0];
            float current = result0.Span[1];
            float power = result0.Span[2];
            float charge = result0.Span[3];
            float temp = result0.Span[4];
            float energy = result0.Span[5];

            Memory<ushort> result1 = await _modbusClient.ReadHoldingRegistersAsync<ushort>(slaveId, 0x000C, 1);
            ushort err_code = result1.Span[0];

            lblVoltage.Text = FormatNum(voltage);
            lblCurrent.Text = FormatNum(current);
            lblPower.Text = FormatNum(power);
            lblCharge.Text = FormatNum(charge);
            lblEnergy.Text = FormatNum(energy);
            lblTemp.Text = FormatNum(temp);
            lblErrcode.Text = $"0x{err_code:X4}";

            string errorDescription = GetErrorMessage(err_code);
            lblErrText.Text = errorDescription;

            if (errorDescription == "正常")
            {
                lblErrText.ForeColor = System.Drawing.Color.Green;
            }
            else
            {
                lblErrText.ForeColor = System.Drawing.Color.Red;
            }
        }
        catch (Exception)
        {
        }
    }

    private async void button2_Click(object sender, EventArgs e)
    {
        if (_modbusClient == null || !_modbusClient.IsConnected)
        {
            MessageBox.Show("请先连接串口！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        try
        {
            _pollTimer.Stop();

            int slaveId = (int)numSlaveId.Value;

            ushort cfgSlaveAddr = ushort.Parse(numModuleAddrConfig.Text);
            float rshunt = float.Parse(textRshuntConfig.Text);
            float imax = float.Parse(textExpImaxConfig.Text);
            float voltageK = float.Parse(textVoltageKConfig.Text);
            float currentK = float.Parse(textCurrentKConfig.Text);
            ushort rshuntPpm =
                ushort.Parse(string.IsNullOrWhiteSpace(textRshuntPpmConfig.Text) ? "0" : textRshuntPpmConfig.Text);
            ushort[] writeRegs = new ushort[17];

            writeRegs[0] = cfgSlaveAddr;

            var (rHigh, rLow) = FloatToRegisters(rshunt);
            writeRegs[1] = rHigh;
            writeRegs[2] = rLow;

            var (iHigh, iLow) = FloatToRegisters(imax);
            writeRegs[3] = iHigh;
            writeRegs[4] = iLow;
            writeRegs[5] = ParseHexOrDec(txtSOVL.Text);
            writeRegs[6] = ParseHexOrDec(txtSUVL.Text);
            writeRegs[7] = ParseHexOrDec(txtBOVL.Text);
            writeRegs[8] = ParseHexOrDec(txtBUVL.Text);
            writeRegs[9] = ParseHexOrDec(txtTempLimit.Text);
            writeRegs[10] = ParseHexOrDec(txtPwrLimit.Text);

            var (vKHigh, vKLow) = FloatToRegisters(voltageK);
            writeRegs[11] = vKHigh;
            writeRegs[12] = vKLow;

            var (cKHigh, cKLow) = FloatToRegisters(currentK);
            writeRegs[13] = cKHigh;
            writeRegs[14] = cKLow;
            writeRegs[15] = rshuntPpm;
            writeRegs[16] = 0x4B56;

            await _modbusClient.WriteMultipleRegistersAsync(slaveId, 0x0100, writeRegs);

            await Task.Delay(500);

            MessageBox.Show("参数已成功写入，需要重新上电以应用参数", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        catch (Exception ex)
        {
            MessageBox.Show($"保存配置失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            if (_isConnected)
            {
                _pollTimer.Start();
            }
        }
    }

    private async Task ReadConfigParametersAsync()
    {
        if (_modbusClient == null || !_modbusClient.IsConnected)
        {
            MessageBox.Show("请先连接串口！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        try
        {
            _pollTimer.Stop();

            byte slaveId = (byte)numSlaveId.Value;
            ushort startAddr = 0x0100;
            ushort regCount = 16;

            Memory<ushort> result = await _modbusClient.ReadHoldingRegistersAsync<ushort>(slaveId, startAddr, regCount);
            ushort[] regs = result.ToArray();

            numModuleAddrConfig.Text = regs[0].ToString();

            float rshunt = RegistersToFloat(regs[1], regs[2]);
            textRshuntConfig.Text = $"{rshunt:F6}";

            float imax = RegistersToFloat(regs[3], regs[4]);
            textExpImaxConfig.Text = $"{imax:F3}";

            txtSOVL.Text = $"0x{regs[5]:X4}";
            txtSUVL.Text = $"0x{regs[6]:X4}";
            txtBOVL.Text = $"0x{regs[7]:X4}";
            txtBUVL.Text = $"0x{regs[8]:X4}";
            txtTempLimit.Text = $"0x{regs[9]:X4}";
            txtPwrLimit.Text = $"0x{regs[10]:X4}";

            float voltageK = RegistersToFloat(regs[11], regs[12]);
            textVoltageKConfig.Text = $"{voltageK:F6}";

            float currentK = RegistersToFloat(regs[13], regs[14]);
            textCurrentKConfig.Text = $"{currentK:F6}";

            textRshuntPpmConfig.Text = regs[15].ToString();
        }
        catch (Exception ex)
        {
            MessageBox.Show($"读取配置失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            if (_isConnected)
            {
                _pollTimer.Start();
            }
        }
    }

    private (ushort High, ushort Low) FloatToRegisters(float value)
    {
        byte[] bytes = BitConverter.GetBytes(value);
        ushort low = (ushort)(bytes[0] | (bytes[1] << 8));
        ushort high = (ushort)(bytes[2] | (bytes[3] << 8));
        return (high, low);
    }

    private float RegistersToFloat(ushort high, ushort low)
    {
        byte[] bytes = new byte[]
        {
            (byte)(low & 0xFF),
            (byte)(low >> 8),
            (byte)(high & 0xFF),
            (byte)(high >> 8)
        };
        return BitConverter.ToSingle(bytes, 0);
    }

    private ushort ParseHexOrDec(string input)
    {
        input = input.Trim();
        if (input.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        {
            return Convert.ToUInt16(input.Substring(2), 16);
        }

        return ushort.Parse(input);
    }

    private string GetErrorMessage(ushort errCode)
    {
        List<string> errors = new List<string>();

        if ((errCode & (1 << 0)) != 0)
        {
            errors.Add("存储器校验失败");
        }

        if ((errCode & (1 << 2)) != 0) errors.Add("功率超限");
        if ((errCode & (1 << 3)) != 0) errors.Add("总线欠压");
        if ((errCode & (1 << 4)) != 0) errors.Add("总线过压");
        if ((errCode & (1 << 5)) != 0) errors.Add("分流欠压");
        if ((errCode & (1 << 6)) != 0) errors.Add("分流过压");
        if ((errCode & (1 << 7)) != 0) errors.Add("温度超限");
        if ((errCode & (1 << 8)) != 0) errors.Add("芯片离线");
        if ((errCode & (1 << 9)) != 0) errors.Add("数学计算溢出");
        if ((errCode & (1 << 10)) != 0) errors.Add("电荷寄存器溢出");
        if ((errCode & (1 << 11)) != 0) errors.Add("能量寄存器溢出");

        if (errors.Count == 0)
        {
            return "正常";
        }

        return string.Join(", ", errors);
    }

    private string FormatNum(float val)
    {
        if (float.IsNaN(val) || float.IsInfinity(val))
        {
            return "0000000000";
        }

        bool isNegative = val < 0;
        float absVal = Math.Abs(val);

        for (int decimals = 4; decimals >= 0; decimals--)
        {
            string formatted;
            if (decimals > 0)
            {
                formatted = absVal.ToString($"F{decimals}");
            }
            else
            {
                formatted = Math.Round(absVal).ToString("F0");
            }

            int maxAllowableLength = isNegative ? 9 : 10;

            if (formatted.Length <= maxAllowableLength)
            {
                string padded = formatted.PadLeft(maxAllowableLength, '0');
                return isNegative ? "-" + padded : padded;
            }
        }

        string strInt = Math.Round(absVal).ToString("F0");
        int targetLen = isNegative ? 9 : 10;

        if (strInt.Length > targetLen)
        {
            strInt = strInt.Substring(0, targetLen);
        }
        else
        {
            strInt = strInt.PadLeft(targetLen, '0');
        }

        return isNegative ? "-" + strInt : strInt;
    }

    private void btnRefresh_Click(object sender, EventArgs e)
    {
        ScanSerialPorts();
    }

    private void btnConnect_Click(object sender, EventArgs e)
    {
        LinkSerialPorts();
    }

    private async void buttonRead_Click(object sender, EventArgs e)
    {
        await ReadConfigParametersAsync();
    }

    private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
    {
        linkLabel1.LinkVisited = true;
        System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo
        {
            FileName = "https://github.com/MDLZCOOL/",
            UseShellExecute = true
        });
    }

    private async void btnUpgrade_Click_1(object sender, EventArgs e)
    {
        if (_modbusClient == null || !_isConnected)
        {
            MessageBox.Show("请先点击“连接”以确认串口和波特率！", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        OpenFileDialog ofd = new OpenFileDialog
        {
            Filter = "Bin Files (*.bin)|*.bin|All Files (*.*)|*.*",
            Title = "选择升级固件"
        };

        if (ofd.ShowDialog() != DialogResult.OK)
        {
            return;
        }

        string filePath = ofd.FileName;
        byte[] fileData = File.ReadAllBytes(filePath);

        string portName = cmbPorts.SelectedItem?.ToString() ?? "";
        int baudRate = int.Parse(cmbBaud.SelectedItem?.ToString() ?? "115200");
        int slaveId = (int)numSlaveId.Value;

        Button btn = (Button)sender;
        string originalText = btn.Text;

        try
        {
            btn.Enabled = false;
            progressBarUpgrade.Value = 0;
            lblProgressPercent.Text = "0%";
            _pollTimer.Stop();

            try
            {
                ushort[] bootRegs = new ushort[] { 0x4B4F };
                await _modbusClient.WriteMultipleRegistersAsync(slaveId, 0x0111, bootRegs);
                await Task.Delay(500);
            }
            catch (Exception)
            {
            }

            CloseSerialPort();

            await Task.Delay(500);

            using (SerialPort serialPort = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One))
            {
                serialPort.Open();
                serialPort.ReadTimeout = 2000;
                serialPort.WriteTimeout = 2000;

                var progress = new Progress<int>(percent =>
                {
                    progressBarUpgrade.Value = percent;
                    lblProgressPercent.Text = $"{percent}%";
                });

                await YmodemSendWithLibraryAsync(serialPort, filePath, fileData, progress);
            }

            MessageBox.Show("固件升级成功！设备将自动重启。\n请重新点击“连接”以恢复通信。", "升级成功", MessageBoxButtons.OK,
                MessageBoxIcon.Information);
        }
        catch (Exception ex)
        {
            MessageBox.Show($"升级失败或被中断: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            btn.Text = originalText;
            btn.Enabled = true;
            progressBarUpgrade.Value = 0;
            lblProgressPercent.Text = "0%";
        }
    }

    private async Task YmodemSendWithLibraryAsync(SerialPort port, string filePath, byte[] fileData,
        IProgress<int> progress)
    {
        var fileDescriptor = new YModemFileDescriptor(Path.GetFileName(filePath), fileData.Length);

        var sender = new YModemSender();
        var encoder = new YModemPacketEncoder();

        int currentOffset = 0;
        bool transferComplete = false;
        YModemEvent pendingEvent = await ReadPeerByteAsync(port);

        while (!transferComplete)
        {
            YModemStepResult stepResult = sender.Advance(pendingEvent);
            pendingEvent = null;

            foreach (YModemAction action in stepResult.Actions)
            {
                switch (action)
                {
                    case YModemAction.RequestFileHeader _:
                        pendingEvent = new YModemEvent.FileHeaderReady(fileDescriptor);
                        break;

                    case YModemAction.RequestDataBlock reqDb:
                        int size = Math.Min(reqDb.BlockSize, fileData.Length - currentOffset);
                        byte[] payload = new byte[size];
                        Array.Copy(fileData, currentOffset, payload, 0, size);

                        bool isLast = (currentOffset + size) >= fileData.Length;

                        pendingEvent = new YModemEvent.DataBlockReady(reqDb.BlockNumber, payload, size, isLast);

                        currentOffset += size;
                        break;

                    case YModemAction.SendPacket sp:
                        byte[] packetData = encoder.Encode(sp.Packet);

                        await Task.Run(() => port.Write(packetData, 0, packetData.Length));

                        int percent = Math.Min(100, (int)((double)currentOffset / fileData.Length * 100));
                        progress?.Report(percent);
                        break;

                    case YModemAction.SendControl sc:
                        byte[] ctrlByte = new byte[] { sc.Value };
                        await Task.Run(() => port.Write(ctrlByte, 0, 1));
                        break;

                    case YModemAction.Complete _:
                        transferComplete = true;
                        progress?.Report(100);
                        break;

                    case YModemAction.Cancel c:
                        throw new Exception($"传输被单片机主动取消: {c.Reason}");

                    case YModemAction.Fail f:
                        throw new Exception($"协议传输失败: {f.Reason}");
                }
            }

            if (!transferComplete && pendingEvent == null)
            {
                pendingEvent = await ReadPeerByteAsync(port);
            }
        }
    }

    private async Task<YModemEvent> ReadPeerByteAsync(SerialPort port)
    {
        return await Task.Run(() =>
        {
            port.ReadTimeout = 3000;
            try
            {
                int b = port.ReadByte();
                return (YModemEvent)new YModemEvent.PeerByteReceived((byte)b);
            }
            catch (TimeoutException)
            {
                throw new Exception("等待单片机回复超时，通信断开。请检查单片机是否正常处于 Bootloader 模式。");
            }
        });
    }
}