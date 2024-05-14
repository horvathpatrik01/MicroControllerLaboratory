namespace LEDMoodlightning
{
    partial class NewAnimForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NewAnimForm));
            this.TB_NewAnim = new System.Windows.Forms.TextBox();
            this.B_Createnewanim = new System.Windows.Forms.Button();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.B_Closecreation = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // TB_NewAnim
            // 
            this.TB_NewAnim.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.TB_NewAnim.Location = new System.Drawing.Point(17, 83);
            this.TB_NewAnim.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.TB_NewAnim.Name = "TB_NewAnim";
            this.TB_NewAnim.Size = new System.Drawing.Size(148, 26);
            this.TB_NewAnim.TabIndex = 0;
            // 
            // B_Createnewanim
            // 
            this.B_Createnewanim.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.B_Createnewanim.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.B_Createnewanim.Location = new System.Drawing.Point(185, 79);
            this.B_Createnewanim.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.B_Createnewanim.Name = "B_Createnewanim";
            this.B_Createnewanim.Size = new System.Drawing.Size(112, 35);
            this.B_Createnewanim.TabIndex = 1;
            this.B_Createnewanim.Text = "Create";
            this.B_Createnewanim.UseVisualStyleBackColor = true;
            // 
            // B_Closecreation
            // 
            this.B_Closecreation.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.B_Closecreation.Location = new System.Drawing.Point(305, 79);
            this.B_Closecreation.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.B_Closecreation.Name = "B_Closecreation";
            this.B_Closecreation.Size = new System.Drawing.Size(112, 35);
            this.B_Closecreation.TabIndex = 2;
            this.B_Closecreation.Text = "Close";
            this.B_Closecreation.UseVisualStyleBackColor = true;
            this.B_Closecreation.MouseClick += new System.Windows.Forms.MouseEventHandler(this.B_Closecreation_MouseClick);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label1.Location = new System.Drawing.Point(13, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(319, 25);
            this.label1.TabIndex = 3;
            this.label1.Text = "Enter the animation name below";
            // 
            // NewAnimForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(439, 142);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.B_Closecreation);
            this.Controls.Add(this.B_Createnewanim);
            this.Controls.Add(this.TB_NewAnim);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "NewAnimForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "New Animation";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox TB_NewAnim;
        private System.Windows.Forms.Button B_Createnewanim;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.Button B_Closecreation;
        private System.Windows.Forms.Label label1;
    }
}