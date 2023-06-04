class Doxide < Formula
  desc "Modern documentation for modern C++. Configure with YAML, output Markdown."
  homepage "https://doxide.org"
  url "https://download.indii.org/brew/doxide-0.0.0.tar.gz"
  version "0.0.0"
  sha256 "0000000000000000000000000000000000000000000000000000000000000000"
  license "Apache-2.0"
  depends_on "cmake" => [:build]
  depends_on "libyaml"

  def install
    system "cmake", "-DCMAKE_BUILD_TYPE=Release", ".", "--verbose"
    system "cmake", "--build", ".", "--config", "Release", "--verbose"
    system "cmake", "--install", ".", "--config", "Release", "--prefix", "#{prefix}", "--verbose"
  end

  test do
    system "true"
  end
end
