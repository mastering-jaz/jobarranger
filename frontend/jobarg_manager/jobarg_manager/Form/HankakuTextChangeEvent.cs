using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using jp.co.ftf.jobcontroller.Common;

namespace jp.co.ftf.jobcontroller.JobController
{
	/// <summary>
    /// TextBoxに、バイト数による入力制限機能を提供するためのクラスです。
    /// </summary>
    public class HankakuTextChangeEvent
    {
        #region フィールド

        /// <summary>
        /// IMEで変換中かどうかのフラグです。
        /// </summary>
        private bool _isImeInput = false;

        /// <summary>
        /// 入力バイト数チェックの結果、テキストの内容を変更したかどうかのフラグです。
        /// イベント内でのテキスト変更による再チェックを防止するために使用します。
        /// </summary>
        private bool _isTextChanged = false;


        #endregion

        #region イベント追加処理

        /// <summary>
        /// 指定されたTextBoxに、IME状態をチェックしつつ入力バイト数制限を行うTextChangedイベントを追加します。
        /// 入力バイト数制限に使用する値は、TextBoxのMaxLengthの値です。
        /// </summary>
        /// <param name="t">イベントを追加するTextBox</param>
        public void AddTextChangedEventHander(TextBox t)
        {
            if (t == null) return;

            TextCompositionManager.AddPreviewTextInputHandler(t, OnPreviewTextInput);
            TextCompositionManager.AddPreviewTextInputStartHandler(t, OnPreviewTextInputStart);
            TextCompositionManager.AddPreviewTextInputUpdateHandler(t, OnPreviewTextInputUpdate);

            t.TextChanged += new TextChangedEventHandler(OnTextChanged);
        }

        #endregion

        #region 入力制限処理

        /// <summary>
        /// TextChangedイベントの処理を行います。
        /// </summary>
        /// <param name="sender">処理対象のTextBox</param>
        /// <param name="e">イベント引数</param>
        private void OnTextChanged(object sender, TextChangedEventArgs e)
        {
            if (_isImeInput) return;

            if (_isTextChanged)
            {
                _isTextChanged = false;
                return;
            }
            TextBox textBox = sender as TextBox;
            Int32 selectionStart = textBox.SelectionStart;
            Int32 selectionLength = textBox.SelectionLength;
            String newText = String.Empty;
            for (int i = 0; i < textBox.Text.Length;i++ )
            {
                String str = textBox.Text.Substring(i, 1);
                if (CheckUtil.EncSJis.GetByteCount(str)==1)
                {
                    newText += str;
                }
            }
            textBox.Text = newText;

            if (textBox == null || CheckUtil.EncSJis.GetByteCount(textBox.Text) <= textBox.MaxLength)
                return;

            _isTextChanged = true;

            textBox.Select(textBox.Text.Length, 0);
        }

        /// <summary>
        /// 指定された文字列から、指定されたバイト数分取得します。
        /// 最終バイト目が全角文字の前半部分の場合、その文字は破棄されます。
        /// </summary>
        /// <param name="target">対象文字列</param>
        /// <param name="byteCount">取得するバイト数</param>
        /// <returns></returns>
        private string getText(string target, int byteCount)
        {
            int len = 0;
            int cnt = 0;

            for (int i = 0; i < target.Length; i++)
            {
                cnt += CheckUtil.EncSJis.GetByteCount(target.Substring(i, 1));
                if (cnt <= byteCount) len++;
            }

            return target.Substring(0, len);
        }

        #endregion

        #region IME状態チェック

        private void OnPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            _isImeInput = false;
        }

        private void OnPreviewTextInputStart(object sender, TextCompositionEventArgs e)
        {
            _isImeInput = true;
        }

        private void OnPreviewTextInputUpdate(object sender, TextCompositionEventArgs e)
        {
            if (e.TextComposition.CompositionText.Length == 0)
                _isImeInput = false;
        }

        #endregion
    }
}



