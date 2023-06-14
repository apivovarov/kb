fn sum(a: i32, b: i32) -> i32 {
    let c = a + b;
    return c;
}

fn main() {
    let a: i32 = 10;
    let b: i32 = 13;
    let c = sum(a, b);
    println!("Hello, world! {}+{}={}", a, b, c);
}
