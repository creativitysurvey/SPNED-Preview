define void @decode_loop(i32* %in, i32* %out, i32 %len) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %next_i, %loop ]
  %in_ptr = getelementptr i32, i32* %in, i32 %i
  %val = load i32, i32* %in_ptr, align 4
  %out_ptr = getelementptr i32, i32* %out, i32 %i
  store i32 %val, i32* %out_ptr, align 4
  %next_i = add i32 %i, 1
  %cond = icmp slt i32 %next_i, %len
  br i1 %cond, label %loop, label %exit
exit:
  ret void
}
