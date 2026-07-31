namespace 电压电流模块上位机;

partial class Form1
{
    /// <summary>
    ///  Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    ///  Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
        if (disposing && (components != null))
        {
            components.Dispose();
        }

        base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
        btnConnect = new System.Windows.Forms.Button();
        button2 = new System.Windows.Forms.Button();
        cmbPorts = new System.Windows.Forms.ComboBox();
        cmbBaud = new System.Windows.Forms.ComboBox();
        buttonRead = new System.Windows.Forms.Button();
        grpComm = new System.Windows.Forms.GroupBox();
        label5 = new System.Windows.Forms.Label();
        label4 = new System.Windows.Forms.Label();
        label3 = new System.Windows.Forms.Label();
        label2 = new System.Windows.Forms.Label();
        txtPollInterval = new System.Windows.Forms.TextBox();
        btnRefresh = new System.Windows.Forms.Button();
        numSlaveId = new System.Windows.Forms.NumericUpDown();
        grpConfig = new System.Windows.Forms.GroupBox();
        lblProgressPercent = new System.Windows.Forms.Label();
        progressBarUpgrade = new System.Windows.Forms.ProgressBar();
        btnUpgrade = new System.Windows.Forms.Button();
        lblVoltage = new System.Windows.Forms.Label();
        groupBox1 = new System.Windows.Forms.GroupBox();
        lblEnergy = new System.Windows.Forms.Label();
        label30 = new System.Windows.Forms.Label();
        lblErrcode = new System.Windows.Forms.Label();
        lblTemp = new System.Windows.Forms.Label();
        label1 = new System.Windows.Forms.Label();
        lblCharge = new System.Windows.Forms.Label();
        lblPower = new System.Windows.Forms.Label();
        lblCurrent = new System.Windows.Forms.Label();
        label14 = new System.Windows.Forms.Label();
        label9 = new System.Windows.Forms.Label();
        label8 = new System.Windows.Forms.Label();
        label7 = new System.Windows.Forms.Label();
        label6 = new System.Windows.Forms.Label();
        lblErrText = new System.Windows.Forms.Label();
        linkLabel1 = new System.Windows.Forms.LinkLabel();
        groupBox2 = new System.Windows.Forms.GroupBox();
        textRshuntPpmConfig = new System.Windows.Forms.TextBox();
        label10 = new System.Windows.Forms.Label();
        textVoltageKConfig = new System.Windows.Forms.TextBox();
        txtPwrLimit = new System.Windows.Forms.TextBox();
        txtTempLimit = new System.Windows.Forms.TextBox();
        textCurrentKConfig = new System.Windows.Forms.TextBox();
        txtBUVL = new System.Windows.Forms.TextBox();
        txtBOVL = new System.Windows.Forms.TextBox();
        txtSUVL = new System.Windows.Forms.TextBox();
        txtSOVL = new System.Windows.Forms.TextBox();
        textExpImaxConfig = new System.Windows.Forms.TextBox();
        textRshuntConfig = new System.Windows.Forms.TextBox();
        numModuleAddrConfig = new System.Windows.Forms.NumericUpDown();
        label26 = new System.Windows.Forms.Label();
        label31 = new System.Windows.Forms.Label();
        label32 = new System.Windows.Forms.Label();
        label33 = new System.Windows.Forms.Label();
        label34 = new System.Windows.Forms.Label();
        label13 = new System.Windows.Forms.Label();
        label18 = new System.Windows.Forms.Label();
        label19 = new System.Windows.Forms.Label();
        label20 = new System.Windows.Forms.Label();
        label21 = new System.Windows.Forms.Label();
        label22 = new System.Windows.Forms.Label();
        groupBox3 = new System.Windows.Forms.GroupBox();
        grpComm.SuspendLayout();
        ((System.ComponentModel.ISupportInitialize)numSlaveId).BeginInit();
        grpConfig.SuspendLayout();
        groupBox1.SuspendLayout();
        groupBox2.SuspendLayout();
        ((System.ComponentModel.ISupportInitialize)numModuleAddrConfig).BeginInit();
        groupBox3.SuspendLayout();
        SuspendLayout();
        // 
        // btnConnect
        // 
        btnConnect.Font = new System.Drawing.Font("Microsoft YaHei UI", 11F);
        btnConnect.Location = new System.Drawing.Point(3, 22);
        btnConnect.Name = "btnConnect";
        btnConnect.Size = new System.Drawing.Size(49, 33);
        btnConnect.TabIndex = 0;
        btnConnect.Text = "连接";
        btnConnect.UseVisualStyleBackColor = true;
        btnConnect.Click += btnConnect_Click;
        // 
        // button2
        // 
        button2.Font = new System.Drawing.Font("Microsoft YaHei UI", 11F);
        button2.Location = new System.Drawing.Point(101, 22);
        button2.Name = "button2";
        button2.Size = new System.Drawing.Size(49, 33);
        button2.TabIndex = 1;
        button2.Text = "保存";
        button2.UseVisualStyleBackColor = true;
        button2.Click += button2_Click;
        // 
        // cmbPorts
        // 
        cmbPorts.FormattingEnabled = true;
        cmbPorts.Location = new System.Drawing.Point(88, 22);
        cmbPorts.Name = "cmbPorts";
        cmbPorts.Size = new System.Drawing.Size(121, 25);
        cmbPorts.TabIndex = 2;
        // 
        // cmbBaud
        // 
        cmbBaud.FormattingEnabled = true;
        cmbBaud.Location = new System.Drawing.Point(325, 22);
        cmbBaud.Name = "cmbBaud";
        cmbBaud.Size = new System.Drawing.Size(121, 25);
        cmbBaud.TabIndex = 3;
        // 
        // buttonRead
        // 
        buttonRead.Font = new System.Drawing.Font("Microsoft YaHei UI", 11F);
        buttonRead.Location = new System.Drawing.Point(52, 22);
        buttonRead.Name = "buttonRead";
        buttonRead.Size = new System.Drawing.Size(49, 33);
        buttonRead.TabIndex = 4;
        buttonRead.Text = "读取";
        buttonRead.UseVisualStyleBackColor = true;
        buttonRead.Click += buttonRead_Click;
        // 
        // grpComm
        // 
        grpComm.Controls.Add(label5);
        grpComm.Controls.Add(label4);
        grpComm.Controls.Add(label3);
        grpComm.Controls.Add(label2);
        grpComm.Controls.Add(txtPollInterval);
        grpComm.Controls.Add(btnRefresh);
        grpComm.Controls.Add(numSlaveId);
        grpComm.Controls.Add(cmbPorts);
        grpComm.Controls.Add(cmbBaud);
        grpComm.Location = new System.Drawing.Point(12, 7);
        grpComm.Name = "grpComm";
        grpComm.Size = new System.Drawing.Size(455, 89);
        grpComm.TabIndex = 5;
        grpComm.TabStop = false;
        grpComm.Text = "通信参数";
        // 
        // label5
        // 
        label5.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label5.Location = new System.Drawing.Point(242, 60);
        label5.Name = "label5";
        label5.Size = new System.Drawing.Size(77, 22);
        label5.TabIndex = 13;
        label5.Text = "查询间隔";
        // 
        // label4
        // 
        label4.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label4.Location = new System.Drawing.Point(6, 58);
        label4.Name = "label4";
        label4.Size = new System.Drawing.Size(76, 22);
        label4.TabIndex = 12;
        label4.Text = "模块地址";
        // 
        // label3
        // 
        label3.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label3.Location = new System.Drawing.Point(254, 22);
        label3.Name = "label3";
        label3.Size = new System.Drawing.Size(65, 22);
        label3.TabIndex = 11;
        label3.Text = "波特率";
        // 
        // label2
        // 
        label2.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label2.Location = new System.Drawing.Point(6, 25);
        label2.Name = "label2";
        label2.Size = new System.Drawing.Size(67, 22);
        label2.TabIndex = 10;
        label2.Text = "COM口";
        // 
        // txtPollInterval
        // 
        txtPollInterval.Location = new System.Drawing.Point(325, 59);
        txtPollInterval.Name = "txtPollInterval";
        txtPollInterval.Size = new System.Drawing.Size(121, 23);
        txtPollInterval.TabIndex = 9;
        // 
        // btnRefresh
        // 
        btnRefresh.Location = new System.Drawing.Point(215, 22);
        btnRefresh.Name = "btnRefresh";
        btnRefresh.Size = new System.Drawing.Size(25, 23);
        btnRefresh.TabIndex = 8;
        btnRefresh.Text = "🔄";
        btnRefresh.UseVisualStyleBackColor = true;
        btnRefresh.Click += btnRefresh_Click;
        // 
        // numSlaveId
        // 
        numSlaveId.Location = new System.Drawing.Point(88, 57);
        numSlaveId.Maximum = new decimal(new int[] { 247, 0, 0, 0 });
        numSlaveId.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
        numSlaveId.Name = "numSlaveId";
        numSlaveId.Size = new System.Drawing.Size(120, 23);
        numSlaveId.TabIndex = 7;
        numSlaveId.Value = new decimal(new int[] { 1, 0, 0, 0 });
        // 
        // grpConfig
        // 
        grpConfig.Controls.Add(lblProgressPercent);
        grpConfig.Controls.Add(progressBarUpgrade);
        grpConfig.Controls.Add(btnUpgrade);
        grpConfig.Controls.Add(button2);
        grpConfig.Controls.Add(btnConnect);
        grpConfig.Controls.Add(buttonRead);
        grpConfig.Location = new System.Drawing.Point(473, 12);
        grpConfig.Name = "grpConfig";
        grpConfig.Size = new System.Drawing.Size(202, 83);
        grpConfig.TabIndex = 6;
        grpConfig.TabStop = false;
        grpConfig.Text = "操作";
        // 
        // lblProgressPercent
        // 
        lblProgressPercent.Font = new System.Drawing.Font("Microsoft YaHei UI", 10F);
        lblProgressPercent.Location = new System.Drawing.Point(150, 57);
        lblProgressPercent.Name = "lblProgressPercent";
        lblProgressPercent.Size = new System.Drawing.Size(46, 18);
        lblProgressPercent.TabIndex = 47;
        lblProgressPercent.Text = "0%";
        lblProgressPercent.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
        // 
        // progressBarUpgrade
        // 
        progressBarUpgrade.Location = new System.Drawing.Point(3, 60);
        progressBarUpgrade.Name = "progressBarUpgrade";
        progressBarUpgrade.Size = new System.Drawing.Size(147, 15);
        progressBarUpgrade.TabIndex = 26;
        // 
        // btnUpgrade
        // 
        btnUpgrade.Font = new System.Drawing.Font("Microsoft YaHei UI", 11F);
        btnUpgrade.Location = new System.Drawing.Point(150, 22);
        btnUpgrade.Name = "btnUpgrade";
        btnUpgrade.Size = new System.Drawing.Size(49, 33);
        btnUpgrade.TabIndex = 5;
        btnUpgrade.Text = "升级";
        btnUpgrade.UseVisualStyleBackColor = true;
        btnUpgrade.Click += btnUpgrade_Click_1;
        // 
        // lblVoltage
        // 
        lblVoltage.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblVoltage.Location = new System.Drawing.Point(59, 19);
        lblVoltage.Name = "lblVoltage";
        lblVoltage.Size = new System.Drawing.Size(96, 23);
        lblVoltage.TabIndex = 7;
        lblVoltage.Text = "-1";
        // 
        // groupBox1
        // 
        groupBox1.Controls.Add(lblEnergy);
        groupBox1.Controls.Add(label30);
        groupBox1.Controls.Add(lblVoltage);
        groupBox1.Controls.Add(lblErrcode);
        groupBox1.Controls.Add(lblTemp);
        groupBox1.Controls.Add(label1);
        groupBox1.Controls.Add(lblCharge);
        groupBox1.Controls.Add(lblPower);
        groupBox1.Controls.Add(lblCurrent);
        groupBox1.Controls.Add(label14);
        groupBox1.Controls.Add(label9);
        groupBox1.Controls.Add(label8);
        groupBox1.Controls.Add(label7);
        groupBox1.Controls.Add(label6);
        groupBox1.Location = new System.Drawing.Point(12, 102);
        groupBox1.Name = "groupBox1";
        groupBox1.Size = new System.Drawing.Size(663, 94);
        groupBox1.TabIndex = 8;
        groupBox1.TabStop = false;
        groupBox1.Text = "数据";
        // 
        // lblEnergy
        // 
        lblEnergy.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblEnergy.Location = new System.Drawing.Point(269, 19);
        lblEnergy.Name = "lblEnergy";
        lblEnergy.Size = new System.Drawing.Size(96, 23);
        lblEnergy.TabIndex = 25;
        lblEnergy.Text = "-1";
        // 
        // label30
        // 
        label30.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label30.Location = new System.Drawing.Point(215, 19);
        label30.Name = "label30";
        label30.Size = new System.Drawing.Size(202, 23);
        label30.TabIndex = 24;
        label30.Text = "能量：                        J";
        // 
        // lblErrcode
        // 
        lblErrcode.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblErrcode.Location = new System.Drawing.Point(495, 19);
        lblErrcode.Name = "lblErrcode";
        lblErrcode.Size = new System.Drawing.Size(96, 23);
        lblErrcode.TabIndex = 23;
        lblErrcode.Text = "-1";
        // 
        // lblTemp
        // 
        lblTemp.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblTemp.Location = new System.Drawing.Point(269, 65);
        lblTemp.Name = "lblTemp";
        lblTemp.Size = new System.Drawing.Size(96, 23);
        lblTemp.TabIndex = 22;
        lblTemp.Text = "-1";
        // 
        // label1
        // 
        label1.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label1.Location = new System.Drawing.Point(6, 19);
        label1.Name = "label1";
        label1.Size = new System.Drawing.Size(203, 23);
        label1.TabIndex = 8;
        label1.Text = "电压：                        V";
        // 
        // lblCharge
        // 
        lblCharge.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblCharge.Location = new System.Drawing.Point(269, 42);
        lblCharge.Name = "lblCharge";
        lblCharge.Size = new System.Drawing.Size(96, 23);
        lblCharge.TabIndex = 21;
        lblCharge.Text = "-1";
        // 
        // lblPower
        // 
        lblPower.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblPower.Location = new System.Drawing.Point(59, 65);
        lblPower.Name = "lblPower";
        lblPower.Size = new System.Drawing.Size(96, 23);
        lblPower.TabIndex = 20;
        lblPower.Text = "-1";
        // 
        // lblCurrent
        // 
        lblCurrent.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblCurrent.Location = new System.Drawing.Point(59, 42);
        lblCurrent.Name = "lblCurrent";
        lblCurrent.Size = new System.Drawing.Size(96, 23);
        lblCurrent.TabIndex = 19;
        lblCurrent.Text = "-1";
        // 
        // label14
        // 
        label14.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label14.Location = new System.Drawing.Point(423, 19);
        label14.Name = "label14";
        label14.Size = new System.Drawing.Size(234, 23);
        label14.TabIndex = 17;
        label14.Text = "错误码：";
        // 
        // label9
        // 
        label9.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label9.Location = new System.Drawing.Point(215, 65);
        label9.Name = "label9";
        label9.Size = new System.Drawing.Size(234, 23);
        label9.TabIndex = 12;
        label9.Text = "温度：                       ℃";
        // 
        // label8
        // 
        label8.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label8.Location = new System.Drawing.Point(215, 42);
        label8.Name = "label8";
        label8.Size = new System.Drawing.Size(234, 23);
        label8.TabIndex = 11;
        label8.Text = "电荷：                        C";
        // 
        // label7
        // 
        label7.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label7.Location = new System.Drawing.Point(6, 65);
        label7.Name = "label7";
        label7.Size = new System.Drawing.Size(203, 23);
        label7.TabIndex = 10;
        label7.Text = "功率：                       W";
        // 
        // label6
        // 
        label6.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label6.Location = new System.Drawing.Point(6, 42);
        label6.Name = "label6";
        label6.Size = new System.Drawing.Size(203, 23);
        label6.TabIndex = 9;
        label6.Text = "电流：                        A";
        // 
        // lblErrText
        // 
        lblErrText.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        lblErrText.Location = new System.Drawing.Point(6, 20);
        lblErrText.Name = "lblErrText";
        lblErrText.Size = new System.Drawing.Size(151, 111);
        lblErrText.TabIndex = 26;
        lblErrText.Text = "连接断开";
        // 
        // linkLabel1
        // 
        linkLabel1.Font = new System.Drawing.Font("Microsoft YaHei UI", 9F, System.Drawing.FontStyle.Italic);
        linkLabel1.Location = new System.Drawing.Point(511, 336);
        linkLabel1.Name = "linkLabel1";
        linkLabel1.Size = new System.Drawing.Size(173, 21);
        linkLabel1.TabIndex = 9;
        linkLabel1.TabStop = true;
        linkLabel1.Text = "Author: MDLZCOOL@Github";
        linkLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
        linkLabel1.LinkClicked += linkLabel1_LinkClicked;
        // 
        // groupBox2
        // 
        groupBox2.Controls.Add(textRshuntPpmConfig);
        groupBox2.Controls.Add(label10);
        groupBox2.Controls.Add(textVoltageKConfig);
        groupBox2.Controls.Add(txtPwrLimit);
        groupBox2.Controls.Add(txtTempLimit);
        groupBox2.Controls.Add(textCurrentKConfig);
        groupBox2.Controls.Add(txtBUVL);
        groupBox2.Controls.Add(txtBOVL);
        groupBox2.Controls.Add(txtSUVL);
        groupBox2.Controls.Add(txtSOVL);
        groupBox2.Controls.Add(textExpImaxConfig);
        groupBox2.Controls.Add(textRshuntConfig);
        groupBox2.Controls.Add(numModuleAddrConfig);
        groupBox2.Controls.Add(label26);
        groupBox2.Controls.Add(label31);
        groupBox2.Controls.Add(label32);
        groupBox2.Controls.Add(label33);
        groupBox2.Controls.Add(label34);
        groupBox2.Controls.Add(label13);
        groupBox2.Controls.Add(label18);
        groupBox2.Controls.Add(label19);
        groupBox2.Controls.Add(label20);
        groupBox2.Controls.Add(label21);
        groupBox2.Controls.Add(label22);
        groupBox2.Location = new System.Drawing.Point(12, 202);
        groupBox2.Name = "groupBox2";
        groupBox2.Size = new System.Drawing.Size(493, 163);
        groupBox2.TabIndex = 24;
        groupBox2.TabStop = false;
        groupBox2.Text = "配置";
        // 
        // textRshuntPpmConfig
        // 
        textRshuntPpmConfig.Location = new System.Drawing.Point(121, 65);
        textRshuntPpmConfig.Name = "textRshuntPpmConfig";
        textRshuntPpmConfig.Size = new System.Drawing.Size(119, 23);
        textRshuntPpmConfig.TabIndex = 46;
        // 
        // label10
        // 
        label10.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label10.Location = new System.Drawing.Point(6, 64);
        label10.Name = "label10";
        label10.Size = new System.Drawing.Size(114, 23);
        label10.TabIndex = 45;
        label10.Text = "采样电阻PPM";
        // 
        // textVoltageKConfig
        // 
        textVoltageKConfig.Location = new System.Drawing.Point(356, 111);
        textVoltageKConfig.Name = "textVoltageKConfig";
        textVoltageKConfig.Size = new System.Drawing.Size(119, 23);
        textVoltageKConfig.TabIndex = 44;
        // 
        // txtPwrLimit
        // 
        txtPwrLimit.Location = new System.Drawing.Point(356, 88);
        txtPwrLimit.Name = "txtPwrLimit";
        txtPwrLimit.Size = new System.Drawing.Size(119, 23);
        txtPwrLimit.TabIndex = 43;
        // 
        // txtTempLimit
        // 
        txtTempLimit.Location = new System.Drawing.Point(356, 65);
        txtTempLimit.Name = "txtTempLimit";
        txtTempLimit.Size = new System.Drawing.Size(119, 23);
        txtTempLimit.TabIndex = 42;
        // 
        // textCurrentKConfig
        // 
        textCurrentKConfig.Location = new System.Drawing.Point(356, 133);
        textCurrentKConfig.Name = "textCurrentKConfig";
        textCurrentKConfig.Size = new System.Drawing.Size(119, 23);
        textCurrentKConfig.TabIndex = 41;
        // 
        // txtBUVL
        // 
        txtBUVL.Location = new System.Drawing.Point(356, 41);
        txtBUVL.Name = "txtBUVL";
        txtBUVL.Size = new System.Drawing.Size(119, 23);
        txtBUVL.TabIndex = 40;
        // 
        // txtBOVL
        // 
        txtBOVL.Location = new System.Drawing.Point(356, 18);
        txtBOVL.Name = "txtBOVL";
        txtBOVL.Size = new System.Drawing.Size(119, 23);
        txtBOVL.TabIndex = 39;
        // 
        // txtSUVL
        // 
        txtSUVL.Location = new System.Drawing.Point(121, 133);
        txtSUVL.Name = "txtSUVL";
        txtSUVL.Size = new System.Drawing.Size(119, 23);
        txtSUVL.TabIndex = 38;
        // 
        // txtSOVL
        // 
        txtSOVL.Location = new System.Drawing.Point(121, 110);
        txtSOVL.Name = "txtSOVL";
        txtSOVL.Size = new System.Drawing.Size(119, 23);
        txtSOVL.TabIndex = 37;
        // 
        // textExpImaxConfig
        // 
        textExpImaxConfig.Location = new System.Drawing.Point(121, 87);
        textExpImaxConfig.Name = "textExpImaxConfig";
        textExpImaxConfig.Size = new System.Drawing.Size(119, 23);
        textExpImaxConfig.TabIndex = 36;
        // 
        // textRshuntConfig
        // 
        textRshuntConfig.Location = new System.Drawing.Point(121, 42);
        textRshuntConfig.Name = "textRshuntConfig";
        textRshuntConfig.Size = new System.Drawing.Size(119, 23);
        textRshuntConfig.TabIndex = 35;
        // 
        // numModuleAddrConfig
        // 
        numModuleAddrConfig.Location = new System.Drawing.Point(121, 19);
        numModuleAddrConfig.Maximum = new decimal(new int[] { 247, 0, 0, 0 });
        numModuleAddrConfig.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
        numModuleAddrConfig.Name = "numModuleAddrConfig";
        numModuleAddrConfig.Size = new System.Drawing.Size(120, 23);
        numModuleAddrConfig.TabIndex = 14;
        numModuleAddrConfig.Value = new decimal(new int[] { 1, 0, 0, 0 });
        // 
        // label26
        // 
        label26.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label26.Location = new System.Drawing.Point(242, 41);
        label26.Name = "label26";
        label26.Size = new System.Drawing.Size(108, 23);
        label26.TabIndex = 25;
        label26.Text = "总线欠压阈值";
        // 
        // label31
        // 
        label31.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label31.Location = new System.Drawing.Point(242, 133);
        label31.Name = "label31";
        label31.Size = new System.Drawing.Size(108, 23);
        label31.TabIndex = 29;
        label31.Text = "电流校准系数";
        // 
        // label32
        // 
        label32.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label32.Location = new System.Drawing.Point(242, 110);
        label32.Name = "label32";
        label32.Size = new System.Drawing.Size(108, 23);
        label32.TabIndex = 28;
        label32.Text = "电压校准系数";
        // 
        // label33
        // 
        label33.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label33.Location = new System.Drawing.Point(242, 87);
        label33.Name = "label33";
        label33.Size = new System.Drawing.Size(108, 23);
        label33.TabIndex = 27;
        label33.Text = "功率超限阈值";
        // 
        // label34
        // 
        label34.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label34.Location = new System.Drawing.Point(242, 64);
        label34.Name = "label34";
        label34.Size = new System.Drawing.Size(108, 23);
        label34.TabIndex = 26;
        label34.Text = "温度超限阈值";
        // 
        // label13
        // 
        label13.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label13.Location = new System.Drawing.Point(6, 19);
        label13.Name = "label13";
        label13.Size = new System.Drawing.Size(108, 23);
        label13.TabIndex = 8;
        label13.Text = "模块地址";
        // 
        // label18
        // 
        label18.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label18.Location = new System.Drawing.Point(242, 17);
        label18.Name = "label18";
        label18.Size = new System.Drawing.Size(108, 23);
        label18.TabIndex = 17;
        label18.Text = "总线过压阈值";
        // 
        // label19
        // 
        label19.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label19.Location = new System.Drawing.Point(6, 133);
        label19.Name = "label19";
        label19.Size = new System.Drawing.Size(108, 23);
        label19.TabIndex = 12;
        label19.Text = "分流欠压阈值";
        // 
        // label20
        // 
        label20.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label20.Location = new System.Drawing.Point(6, 110);
        label20.Name = "label20";
        label20.Size = new System.Drawing.Size(108, 23);
        label20.TabIndex = 11;
        label20.Text = "分流过压阈值";
        // 
        // label21
        // 
        label21.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label21.Location = new System.Drawing.Point(6, 87);
        label21.Name = "label21";
        label21.Size = new System.Drawing.Size(108, 23);
        label21.TabIndex = 10;
        label21.Text = "最大预期电流";
        // 
        // label22
        // 
        label22.Font = new System.Drawing.Font("Microsoft YaHei UI", 12F);
        label22.Location = new System.Drawing.Point(6, 42);
        label22.Name = "label22";
        label22.Size = new System.Drawing.Size(108, 23);
        label22.TabIndex = 9;
        label22.Text = "采样电阻阻值";
        // 
        // groupBox3
        // 
        groupBox3.Controls.Add(lblErrText);
        groupBox3.Location = new System.Drawing.Point(511, 202);
        groupBox3.Name = "groupBox3";
        groupBox3.Size = new System.Drawing.Size(164, 134);
        groupBox3.TabIndex = 25;
        groupBox3.TabStop = false;
        groupBox3.Text = "错误信息";
        // 
        // Form1
        // 
        AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
        AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        ClientSize = new System.Drawing.Size(687, 370);
        Controls.Add(groupBox3);
        Controls.Add(linkLabel1);
        Controls.Add(groupBox2);
        Controls.Add(groupBox1);
        Controls.Add(grpConfig);
        Controls.Add(grpComm);
        Text = "电压电流模块上位机";
        Load += Form1_Load;
        grpComm.ResumeLayout(false);
        grpComm.PerformLayout();
        ((System.ComponentModel.ISupportInitialize)numSlaveId).EndInit();
        grpConfig.ResumeLayout(false);
        groupBox1.ResumeLayout(false);
        groupBox2.ResumeLayout(false);
        groupBox2.PerformLayout();
        ((System.ComponentModel.ISupportInitialize)numModuleAddrConfig).EndInit();
        groupBox3.ResumeLayout(false);
        ResumeLayout(false);
    }

    private System.Windows.Forms.Label lblProgressPercent;

    private System.Windows.Forms.ProgressBar progressBarUpgrade;

    private System.Windows.Forms.Button btnUpgrade;

    private System.Windows.Forms.TextBox textRshuntPpmConfig;
    private System.Windows.Forms.Label label10;

    private System.Windows.Forms.GroupBox groupBox3;

    private System.Windows.Forms.TextBox txtBOVL;
    private System.Windows.Forms.TextBox txtBUVL;
    private System.Windows.Forms.TextBox textCurrentKConfig;
    private System.Windows.Forms.TextBox txtTempLimit;
    private System.Windows.Forms.TextBox txtPwrLimit;
    private System.Windows.Forms.TextBox textVoltageKConfig;

    private System.Windows.Forms.TextBox textRshuntConfig;
    private System.Windows.Forms.NumericUpDown numModuleAddrConfig;
    private System.Windows.Forms.TextBox textExpImaxConfig;
    private System.Windows.Forms.TextBox txtSOVL;
    private System.Windows.Forms.TextBox txtSUVL;

    private System.Windows.Forms.Label lblErrText;

    private System.Windows.Forms.Label lblEnergy;
    private System.Windows.Forms.Label label30;

    private System.Windows.Forms.Label label26;
    private System.Windows.Forms.Label label31;
    private System.Windows.Forms.Label label32;
    private System.Windows.Forms.Label label33;
    private System.Windows.Forms.Label label34;

    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.Label label13;
    private System.Windows.Forms.Label label18;
    private System.Windows.Forms.Label label19;
    private System.Windows.Forms.Label label20;
    private System.Windows.Forms.Label label21;
    private System.Windows.Forms.Label label22;

    private System.Windows.Forms.LinkLabel linkLabel1;

    private System.Windows.Forms.Label label14;
    private System.Windows.Forms.Label lblCurrent;
    private System.Windows.Forms.Label lblPower;
    private System.Windows.Forms.Label lblCharge;
    private System.Windows.Forms.Label lblTemp;
    private System.Windows.Forms.Label lblErrcode;

    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.Label label9;

    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label label5;

    private System.Windows.Forms.Label label3;

    private System.Windows.Forms.Label label2;

    private System.Windows.Forms.TextBox txtPollInterval;

    private System.Windows.Forms.Button btnRefresh;

    private System.Windows.Forms.Label label1;

    private System.Windows.Forms.GroupBox groupBox1;

    private System.Windows.Forms.Label lblVoltage;

    private System.Windows.Forms.NumericUpDown numSlaveId;

    private System.Windows.Forms.GroupBox grpConfig;

    private System.Windows.Forms.GroupBox grpComm;

    private System.Windows.Forms.Button buttonRead;

    private System.Windows.Forms.ComboBox cmbBaud;

    private System.Windows.Forms.ComboBox cmbPorts;

    private System.Windows.Forms.Button button2;

    private System.Windows.Forms.Button btnConnect;

    #endregion
}