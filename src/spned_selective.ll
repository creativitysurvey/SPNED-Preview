define void @decode_selective(i32* %in, i32* %out, i32 %len) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %next_i, %continue ]
  %out_idx = phi i32 [ 0, %entry ], [ %next_out_idx, %continue ]
  %in_ptr = getelementptr i32, i32* %in, i32 %i
  %val = load i32, i32* %in_ptr, align 4
  %rem = srem i32 %val, 10
  %cond = icmp eq i32 %rem, 0
  br i1 %cond, label %match, label %continue
match:
  %out_ptr = getelementptr i32, i32* %out, i32 %out_idx
  store i32 %val, i32* %out_ptr, align 4
  %inc_out = add i32 %out_idx, 1
  br label %continue
continue:
  %next_out_idx = phi i32 [ %out_idx, %loop ], [ %inc_out, %match ]
  %next_i = add i32 %i, 1
  %loop_cond = icmp slt i32 %next_i, %len
  br i1 %loop_cond, label %loop, label %exit
exit:
  ret void
}
