class Doxide < Formula
  desc "Modern documentation for modern C++. Configure with YAML, output Markdown."
  homepage "https://doxide.org"
  url "https://download.indii.org/homebrew/doxide-0.0.0.tar.gz"
  version "0.0.0"
  sha256 "0000000000000000000000000000000000000000000000000000000000000000"
  license "Apache-2.0"
  depends_on "cmake" => [:build]
  depends_on "libyaml"
  depends_on "tree-sitter"

  def install
    system "cmake", "-DCMAKE_BUILD_TYPE=Release", "."
    system "cmake", "--build", ".", "--config", "Release"
    system "cmake", "--install", ".", "--config", "Release", "--prefix", "#{prefix}"
  end

  test do
    system "true"
  end
end
